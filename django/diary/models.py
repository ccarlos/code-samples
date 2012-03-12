import datetime

from django.conf import settings
from django.contrib.auth.models import User
from django.core.urlresolvers import reverse
from django.db import models
from django.db.models.signals import pre_delete

from tidings.models import NotificationsMixin

from notify.models import NoticeMixin
from notify.notices import bind_delete_signal
from sowink.helpers import urlparams
from sowink.models import ModelBase, ManagerBase
from upload.utils import auto_delete_files


class DiaryManager(ManagerBase):
    """Adds diary-specific functions to query entries."""
    use_for_related_fields = True

    def get_for_month(self, creator, visitor, year, month):
        """Diary entries visible to `visitor` for given year and month.

        Fall back to today's year/month if not specified.

        """
        kwargs = {}
        if visitor != creator:
            kwargs.update({'is_draft': False, 'is_private': False})
        return creator.diaries.filter(created__year=year,
                                      created__month=month,
                                      **kwargs).order_by('-created')

    def get_all(self, creator, visitor):
        """Diary entries visible to `visitor` for entire history."""
        kwargs = {}
        if visitor != creator:
            kwargs.update({'is_draft': False, 'is_private': False})
        return creator.diaries.filter(**kwargs).order_by('-created')


class DiaryEntry(NoticeMixin, NotificationsMixin, ModelBase):
    creator = models.ForeignKey(User, related_name='diaries')
    created = models.DateTimeField(db_index=True,
                                   default=datetime.datetime.now)
    created_day = models.DateField(db_index=True, verbose_name='Day',
                                   default=datetime.date.today)
    text = models.TextField(max_length=10000)
    is_draft = models.BooleanField(default=False, db_index=True,
                                   verbose_name='Is draft?')
    is_private = models.BooleanField(default=False, db_index=True,
                                     verbose_name='Is private?')
    updated = models.DateTimeField(default=datetime.datetime.now)

    objects = DiaryManager()

    def __unicode__(self):
        return u"%s's entry on %s" % (self.creator.username, self.created_day)

    def can_edit(self, user):
        return self.creator == user

    def get_absolute_url(self):
        return reverse('diary.single',
            kwargs={'username': self.creator.username,
                    'year': self.created_day.year,
                    'month': self.created_day.month,
                    'day': self.created_day.day})

    class Meta(object):
        ordering = ['-created']
        unique_together = ('creator', 'created_day')
        verbose_name = 'Diary entry'
        verbose_name_plural = 'Diary entries'


pre_delete.connect(bind_delete_signal, sender=DiaryEntry,
                                         dispatch_uid='delete_diaryentry')


class Comment(ModelBase):
    diary = models.ForeignKey(DiaryEntry, related_name='comments')
    text = models.TextField(max_length=1000, verbose_name='Content')
    creator = models.ForeignKey(User, related_name='posted_comments')
    created = models.DateTimeField(db_index=True,
                                   default=datetime.datetime.now)

    def get_absolute_url(self):
        url_ = self.diary.get_absolute_url()
        return urlparams(url_, hash='comment-%s' % self.id)

    class Meta:
        ordering = ['created']
        verbose_name = 'Diary comment'
        verbose_name_plural = 'Diary comments'


@auto_delete_files
class DiaryPicture(ModelBase):
    entry = models.ForeignKey(DiaryEntry, related_name='pictures')
    image = models.ImageField(upload_to=settings.DIARY_IMAGE_PATH,
                              max_length=settings.MAX_FILEPATH_LENGTH)
    thumbnail = models.ImageField(upload_to=settings.DIARY_THUMBNAIL_PATH,
                                  max_length=settings.MAX_FILEPATH_LENGTH)
    created = models.DateTimeField(db_index=True,
                                   default=datetime.datetime.now)

    def __unicode__(self):
        return u"%s pic upload on %s" % (self.entry, self.created)
