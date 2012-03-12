import calendar
from datetime import datetime, date
import os

from django.conf import settings
from django.core.files import File
from django.shortcuts import get_object_or_404

from diary.models import DiaryEntry, DiaryPicture
from diary.tasks import generate_entry_thumbnail
from sowink.utils import smart_int
from upload.utils import resize_image


MONTHS = ['January', 'February', 'March', 'April', 'May', 'June', 'July',
          'August', 'September', 'October', 'November', 'December']


def clean_diary_date(year=None, month=None):
    """For a year and a month, return valid year/month."""
    today = datetime.today()
    year = smart_int(year) or today.year
    month = smart_int(month) or today.month
    return year, max(min(month, 12), 1)


def get_kwargs_for_diary_id(diary_id):
    diary = get_object_or_404(DiaryEntry, pk=diary_id)
    return {'year': diary.created.year,
            'username': diary.creator.username,
            'month': diary.created.month,
            'day': diary.created.day}


def get_diary_calendar(entries, year, month):
    return {'months': MONTHS,
            'current': {'year': year, 'month': month},
            'today': datetime.today(),
            'days_in_month': calendar.monthrange(year, month)[1],
            'days_with_entries': days_with_entries(entries, year, month)}


def diary_today_exists(entries):
    """Has the user written an entry today?"""
    if (entries and entries[0].created_day == date.today() and
        entries[0].is_draft == False):
        return True
    return False


def draft_today_exists(entries):
    """Has the user written a draft today?"""
    if (entries and entries[0].created_day == date.today() and
        entries[0].is_draft == True):
        return True
    return False


def days_with_entries(entries, year, month):
    """Create a dict that maps days(int) to diary entries."""
    days_with_entries = {}
    for e in entries:
        if e.created_day.year == year and e.created_day.month == month:
            days_with_entries[e.created.day] = e
    return days_with_entries


def create_diary_pic(up_file, entry):
    """
    Given an uploaded file and a diary entry, it creates a DiaryPicture
    belonging to an entry.
    """
    diary_pic = DiaryPicture(entry=entry)
    img = File(up_file)
    diary_pic.image.save(up_file.name, img, save=False)
    old_path = diary_pic.image.path
    if os.path.exists(old_path):
        resized = resize_image(diary_pic.image.path,
                               size=settings.DIARY_IMAGE_SIZE)
        os.unlink(old_path)
        diary_pic.image.save(diary_pic.image.name, resized, save=False)

    diary_pic.thumbnail.save(up_file.name, img, save=False)
    diary_pic.save()

    # Generate thumbnail off thread
    generate_entry_thumbnail.delay(diary_pic.pk)

    return diary_pic
