from datetime import datetime

from django.conf import settings
from django.contrib.auth.models import User
from django.db import models

from lets import CAT_OTHR, CATEGORY_CHOICES, LET_ICON_CLASS, LET_ICON
from messages.models import Thread
from sowink.helpers import url, urlparams
from sowink.models import ModelBase


class Lets(ModelBase):
    """Keep track of user's "let's" activites."""
    creator = models.ForeignKey(User, related_name=u'lets')
    content = models.TextField(max_length=100)
    category = models.IntegerField(default=CAT_OTHR, choices=CATEGORY_CHOICES)
    created = models.DateTimeField(db_index=True, default=datetime.now)
    updated = models.DateTimeField(default=datetime.now)

    def get_category_class(self):
        return LET_ICON_CLASS[self.category]

    def get_category_img(self, large=False):
        ends = LET_ICON[self.category]
        if large:
            ends = ends.replace('lets/', 'lets/l/')
        return settings.MEDIA_URL + ends

    def get_category_url(self, base=None, absolute=False):
        if not base:
            base = url('lets', absolute=absolute)
        return urlparams(base, category=self.category)

    @classmethod
    def u_cache_key(cls, creator):
        return 'lets_coins_%s' % creator.pk

    def __unicode__(self):
        return u'%s wants to...(@ %s)' % (self.creator.username, self.created)

    class Meta(object):
        ordering = ['-created']
        verbose_name_plural = "Lets"


class LetsVote(ModelBase):
    creator = models.ForeignKey(User, related_name=u'let_votes_created')
    lets_obj = models.ForeignKey(Lets, related_name=u'let_votes_received')
    created = models.DateTimeField(db_index=True, default=datetime.now)

    def __unicode__(self):
        return u'%s voted %s' % (
            self.creator.username, self.lets_obj)

    class Meta(object):
        unique_together = ('creator', 'lets_obj')


class LetsMap(ModelBase):
    """Map between Lets object and Thread."""
    lets = models.ForeignKey(Lets, related_name='maps')
    thread = models.ForeignKey(Thread, related_name='lets_map')
    created = models.DateTimeField(db_index=True, default=datetime.now)

    class Meta(object):
        unique_together = ('lets', 'thread')

    def __unicode__(self):
        return '%s -FOR- %s' % (self.lets, self.thread)
