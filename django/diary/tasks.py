import os
from django.conf import settings

from celery.decorators import task

from diary.models import DiaryPicture
from upload.utils import resize_image


@task
def generate_entry_thumbnail(diary_pic_id):
    try:
        diary_pic = DiaryPicture.uncached.get(pk=diary_pic_id)
    except DiaryPicture.DoesNotExist:
        return

    resized = resize_image(diary_pic.thumbnail.path,
                           size=settings.DIARY_THUMBNAIL_SIZE)
    # Remove old image.
    os.unlink(diary_pic.thumbnail.path)
    diary_pic.thumbnail.save(diary_pic.thumbnail.name, resized, save=True)
