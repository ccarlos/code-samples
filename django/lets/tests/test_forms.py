from nose.tools import eq_

from lets import forms
from lets.models import CAT_OTHR
from sowink.tests import TestCase
from users.tests import get_user

LETS_TEXT = 's' * 50
LETS_LONG_TEXT = 's' * 101


class LetsFormTests(TestCase):
    def setUp(self):
        super(LetsFormTests, self).setUp()
        self.c = get_user(username='ccarlos')

    def test_new_entry(self):
        f = forms.LetsForm(creator=self.c, data={'content': LETS_TEXT,
                                                 'category': CAT_OTHR})
        assert f.is_valid()

        let_entry = f.save()
        eq_(let_entry.category, CAT_OTHR)

    def test_new_entry_trimmed(self):
        texts = ["  Let's..." + LETS_TEXT,
                 "LETS" + LETS_TEXT,
                 "LET'S" + LETS_TEXT,
                 " LET'S:!@(@" + LETS_TEXT,
                 "Let's..." + LETS_TEXT,
                 "Let's." + LETS_TEXT,
                 "Let's " + LETS_TEXT]
        for text in texts:
            f = forms.LetsForm(creator=self.c, data={'content': text,
                                                     'category': CAT_OTHR})
            assert f.is_valid()
            eq_(LETS_TEXT, f.cleaned_data['content'])

    def test_new_entry_no_category(self):
        f = forms.LetsForm(creator=self.c, data={'content': LETS_TEXT,
                                                 'category': ''})
        assert f.is_valid()

        let_entry = f.save()
        eq_(let_entry.category, CAT_OTHR)

    def test_no_content(self):
        f = forms.LetsForm(creator=self.c, data={'content': '',
                                                 'category': CAT_OTHR})
        assert not f.is_valid()
        eq_(f.errors['content'][0], u'Please include a message.')

    def test_long_content(self):
        f = forms.LetsForm(creator=self.c, data={'content': LETS_LONG_TEXT,
                                                 'category': CAT_OTHR})
        assert not f.is_valid()
        eq_(f.errors['content'][0],
            u'Please keep your message under 100 characters'
            ' (currently 101 characters.')

    def test_short_content(self):
        f = forms.LetsForm(creator=self.c, data={'content': 's',
                                                 'category': CAT_OTHR})
        assert not f.is_valid()
        eq_(f.errors['content'][0], u'Your message is too short.')
