from django.core.exceptions import PermissionDenied
from django.shortcuts import get_object_or_404

from nose.tools import eq_
from test_utils import RequestFactory

from django.core.urlresolvers import reverse

from diary import forms
from diary import views
from diary.models import Comment
from diary.utils import get_kwargs_for_diary_id
from sowink.tests import TestCase, contains, not_contains
from users.tests import get_user


class ViewsTests(TestCase):
    def setUp(self):
        super(ViewsTests, self).setUp()
        self.factory = RequestFactory()
        self.c = get_user('CTO')
        self.u = get_user('zinoma')
        self.f = forms.DiaryForm(creator=self.u, data={'text': 's' * 500})
        self.e = self.f.save()

    def test_new_entry(self):
        # Since user doesn't have an entry, we check if the
        # new diary form is displayed.
        url = reverse('diary.new')
        request = self.factory.get(url)
        request.user = self.c
        response = views.new(request)
        eq_(response.status_code, 200)
        page_content = response.content
        contains(page_content, 'Write your entry here...')
        contains(page_content, 'action=\"/diary/new')

    def test_existing_entry(self):
        url = reverse('diary.list_all', args=[self.u.username])
        request = self.factory.get(url)
        request.user = self.u
        response = views.list_personal(request, username=self.u.username)
        eq_(response.status_code, 200)
        page_content = response.content
        contains(page_content, '/diary/user/zinoma/%d/%d/%d' %
            (self.e.created_day.year, self.e.created_day.month,
             self.e.created_day.day))
        contains(page_content, 'entry/%d/delete' % (self.e.id))

    def test_edit_entry(self):
        url = reverse('diary.edit', args=[self.e.id])
        request = self.factory.get(url)
        request.user = self.u
        response = views.edit(request, diary_id=self.e.id)
        eq_(response.status_code, 200)
        page_content = response.content
        # In edit form box, assert initial contents are displayed.
        contains(page_content, 's' * 500)
        contains(page_content, 'diary/entry/%d/edit' % (self.e.id))

    def test_edit_someones_entry(self):
        """Edit someone else's entry."""
        url = reverse('diary.edit', args=[self.e.id])
        request = self.factory.get(url)
        request.user = self.c
        fn = lambda: views.edit(request, diary_id=self.e.id)
        self.assertRaises(PermissionDenied, fn)

    def test_edit_update_entry(self):
        url = reverse('diary.edit', args=[self.e.id])
        request = self.factory.post(url, {'text': 'x' * 500})
        request.user = self.u
        response = views.edit(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)

        # Was the diary entry changed?
        url = reverse('diary.list_all', args=[self.u.username])
        request = self.factory.get(url)
        request.user = self.u
        response = views.list_personal(request, username=self.u.username)
        eq_(response.status_code, 200)
        # Yes, indeed.
        contains(response.content, 'x' * 500)

    def test_reply_diary(self):
        url = reverse('diary.reply', args=[self.e.id])
        request = self.factory.post(url, {'text': 'ha' * 10})
        request.user = self.u
        response = views.reply(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)

        # Was the diary comment created?
        url = reverse('diary.single',
                      kwargs=get_kwargs_for_diary_id(self.e.id))
        request = self.factory.get(url)
        request.user = self.u
        response = views.single(request, username=self.u.username,
                                year=self.e.created_day.year,
                                month=self.e.created_day.month,
                                day=self.e.created_day.day)
        eq_(response.status_code, 200)
        # Yes, indeed.
        contains(response.content, 'ha' * 10)

    def test_delete_own_reply(self):
        """Delete own comment on owned diary."""
        url = reverse('diary.reply', args=[self.e.id])
        request = self.factory.post(url, {'text': 'ha' * 10})
        request.user = self.u
        response = views.reply(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)
        assert Comment.objects.filter(diary=self.e.id, creator=self.u).exists()

        comm = get_object_or_404(Comment, diary=self.e.id, creator=self.u)
        url = reverse('diary.delete_comment', args=[comm.id])
        request = self.factory.post(url)
        request.user = self.u
        response = views.delete_comment(request, comment_id=comm.id)
        assert not (Comment.objects.filter(diary=self.e.id, creator=self.u).
                    exists())

    def test_delete_someones_reply(self):
        """Delete someone else's comment as the owner of the entry."""
        url = reverse('diary.reply', args=[self.e.id])
        request = self.factory.post(url, {'text': 'ha' * 10})
        request.user = self.c
        response = views.reply(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)
        assert Comment.objects.filter(diary=self.e.id, creator=self.c).exists()

        comm = get_object_or_404(Comment, diary=self.e.id, creator=self.c)
        url = reverse('diary.delete_comment', args=[comm.id])
        request = self.factory.post(url)
        request.user = self.u
        response = views.delete_comment(request, comment_id=comm.id)
        assert not (Comment.objects.filter(diary=self.e.id, creator=self.c).
                    exists())

    def test_delete_someones_reply_not_owner(self):
        """Delete someone else's comment as NOT the owner of the entry."""
        url = reverse('diary.reply', args=[self.e.id])
        request = self.factory.post(url, {'text': 'ha' * 10})
        request.user = self.u
        response = views.reply(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)
        assert Comment.objects.filter(diary=self.e.id, creator=self.u).exists()

        comm = get_object_or_404(Comment, diary=self.e.id, creator=self.u)
        url = reverse('diary.delete_comment', args=[comm.id])
        request = self.factory.post(url)
        request.user = self.c
        fn = lambda: views.delete_comment(request, comment_id=comm.id)
        self.assertRaises(PermissionDenied, fn)

    def test_delete_own_reply_diff_diary(self):
        """Delete own comment as NOT the owner of the entry."""
        url = reverse('diary.reply', args=[self.e.id])
        request = self.factory.post(url, {'text': 'ha' * 10})
        request.user = self.c
        response = views.reply(request, diary_id=self.e.id)
        # Is HttpResponseRedirect called (based on a valid form)?
        eq_(response.status_code, 302)
        assert Comment.objects.filter(diary=self.e.id, creator=self.c).exists()

        comm = get_object_or_404(Comment, diary=self.e.id, creator=self.c)
        url = reverse('diary.delete_comment', args=[comm.id])
        request = self.factory.post(url)
        request.user = self.c
        response = views.delete_comment(request, comment_id=comm.id)
        assert not (Comment.objects.filter(diary=self.e.id, creator=self.c).
                    exists())

    def test_delete_existing_entry(self):
        url = reverse('diary.delete', args=[self.e.id])
        request = self.factory.post(url)
        request.user = self.u
        response = views.delete(request, diary_id=self.e.id)
        # Assert a redirect took place.
        eq_(response.status_code, 302)

        # Was the diary entry deleted?
        url = reverse('diary.list_all', args=[self.u.username])
        request = self.factory.get(url)
        request.user = self.u
        response = views.list_personal(request, username=self.u.username)
        eq_(response.status_code, 200)
        # Yes, indeed.
        not_contains(response.content, 'a' * 500)

    def test_delete_someones_entry(self):
        """Delete someone else's entry."""
        url = reverse('diary.delete', args=[self.e.id])
        request = self.factory.post(url)
        request.user = self.c
        fn = lambda: views.delete(request, diary_id=self.e.id)
        self.assertRaises(PermissionDenied, fn)
