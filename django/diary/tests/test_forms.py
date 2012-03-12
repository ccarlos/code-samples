from nose.tools import eq_

from diary import forms
from sowink.tests import TestCase
from users.tests import get_user

DIARY_TEXT = 's' * 500
DIARY_LONG_TEXT = 's' * 10001
COM_TEXT = 's' * 10
COM_LONG_TEXT = 's' * 2001


class DiaryFormTests(TestCase):
    def setUp(self):
        super(DiaryFormTests, self).setUp()
        self.c = get_user(username='ccarlos')

    def test_new_entry(self):
        f = forms.DiaryForm(creator=self.c, data={'text': DIARY_TEXT})
        assert f.is_valid()

    def test_no_content(self):
        f = forms.DiaryForm(creator=self.c, data={'text': ''})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Please provide some content.')

    def test_long_content(self):
        f = forms.DiaryForm(creator=self.c, data={'text': DIARY_LONG_TEXT})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Your entry is too long (10,001 characters).'
                                 ' It should be at most 10,000 characters.')

    def test_short_content(self):
        f = forms.DiaryForm(creator=self.c, data={'text': 's'})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Your entry is too short (1 characters).'
                                 ' It should be at least 50 characters.')


class CommentFormTests(TestCase):
    def setUp(self):
        super(CommentFormTests, self).setUp()
        self.c = get_user(username='ccarlos')
        self.u = get_user(username='CTO')
        # TODO: Create a valid diary entry in the test database
        self.d = forms.DiaryForm(creator=self.c, data={'text': DIARY_TEXT})
        self.d = self.d.save()

    def test_new_comment(self):
        f = forms.CommentForm(creator=self.u, diary=self.d,
                              data={'text': COM_TEXT})
        assert f.is_valid()

    def test_no_comment(self):
        f = forms.CommentForm(creator=self.u, diary=self.d, data={'text': ''})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Please provide some content.')

    def test_long_comment(self):
        f = forms.CommentForm(creator=self.u, diary=self.d,
                              data={'text': COM_LONG_TEXT})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Your entry is too long (2,001 characters).'
                                 ' It should be at most 2,000 characters.')

    def test_short_comment(self):
        f = forms.CommentForm(creator=self.c, diary=self.d, data={'text': 's'})
        assert not f.is_valid()
        eq_(f.errors['text'][0], u'Your entry is too short (1 characters).'
                                 ' It should be at least 10 characters.')
