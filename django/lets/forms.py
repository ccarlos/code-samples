import logging
import re

from django.core.cache import cache
from django import forms

from lets.models import Lets, CAT_OTHR
from sowink.form_fields import IntegerField, StrippedCharField
import users as ucs


log = logging.getLogger('s.lets')


LETS_MSG_LONG = (u'Please keep your message under %(limit_value)s '
                 u'characters (currently %(show_value)s characters.')
LETS_MSG_SHORT = u'Your message is too short.'
LETS_MSG_REQ = u'Please include a message.'

LETS_RE = re.compile(r"^Let'?s[^a-z]*", re.IGNORECASE | re.DOTALL)


class LetsForm(forms.ModelForm):
    category = IntegerField(required=False)
    content = StrippedCharField(
        required=True, min_length=2, max_length=100,
        error_messages={'required': LETS_MSG_REQ,
                        'min_length': LETS_MSG_SHORT,
                        'max_length': LETS_MSG_LONG})

    def __init__(self, creator, data=None, *args, **kwargs):
        self.creator = creator
        if data:
            data = data.copy()
            data.update({'creator': creator.pk})
        super(LetsForm, self).__init__(data=data, *args, **kwargs)

    def clean_category(self):
        return self.cleaned_data.get('category') or CAT_OTHR

    def clean_content(self):
        c = self.cleaned_data.get('content', '')
        return LETS_RE.sub('', c)

    def save(self, commit=True, *args, **kwargs):
        lets_object = super(LetsForm, self).save(
            *args, commit=commit, **kwargs)

        if not commit:
            return lets_object

        lets_object.save()

        # Coins
        user_key = Lets.u_cache_key(self.creator)
        lets_key = cache.get(user_key)
        if lets_key:
            # No 250 +
            cache.incr(user_key)
        else:
            # Yes 250 +
            cache.set(user_key, 1, 86400)
            profile = self.creator.profile
            old_coins = profile.coins
            profile.update(coins=profile.coins +
                           ucs.ACTIONS_COINS['lets']['daily'])
            updated_coins = profile.coins
            changetuple = (self.creator.username, old_coins, updated_coins)
            log.warning('Coins changed for (%s): %s -> %s' % changetuple)

        return lets_object

    class Meta(object):
        model = Lets
        exclude = ['created', 'updated']
