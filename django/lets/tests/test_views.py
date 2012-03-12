from django.core.exceptions import PermissionDenied

from nose.tools import eq_
from test_utils import RequestFactory

from django.core.urlresolvers import reverse

from lets import forms, models, views
from mojo import IPM_POINTS
from sowink.tests import TestCase
from users.tests import get_user
from users.utils import get_users_and_visits


CAT_OTHR = models.CAT_OTHR


class ViewsTests(TestCase):
    def setUp(self):
        super(ViewsTests, self).setUp()
        self.factory = RequestFactory()
        self.u = get_user('ccarlos')
        self.c = get_user('CTO')

    def _create_lets(self):
        f = forms.LetsForm(creator=self.u, data={'content': 'Hello',
                                                 'category': CAT_OTHR})
        return f.save()

    def test_delete_own_lets(self):
        e = self._create_lets()
        url = reverse('diary.delete', args=[e.id])
        request = self.factory.post(url)
        request.user = self.u
        response = views.delete(request, lets_id=e.id)
        eq_(response.status_code, 302)

        # Assert object was deleted.
        assert not models.Lets.objects.filter(creator=self.u).exists()

    def test_delete_someones_entry(self):
        """Delete someone else's lets object."""
        e = self._create_lets()
        url = reverse('lets.delete', args=[e.id])
        request = self.factory.post(url)
        request.user = self.c
        fn = lambda: views.delete(request, lets_id=e.id)
        self.assertRaises(PermissionDenied, fn)

    def test_vote_ipm(self):
        visits = get_users_and_visits(self.u, self.c)[2]
        before = visits[0].mojo

        e = self._create_lets()
        url = reverse('lets.like', args=[e.id])
        request = self.factory.post(url)
        request.user = self.c
        views.like_lets(request, lets_id=e.id)

        visits = get_users_and_visits(self.u, self.c)[2]
        after = visits[0].mojo
        eq_(before + IPM_POINTS['lets_like']['per'], after)

    def test_view_activities(self):
        from users.forms import SettingsForm
        from users.models import Setting

        request = self.factory.get(reverse('lets'))
        request.user = self.u
        views.activities(request)

        eq_('lets', Setting.get_for_user(self.u, 'home_tab'))

        form = SettingsForm(data={'home_tab': 'stream'})
        form.is_valid()
        form.save_for_user(request.user)
        eq_('stream', Setting.get_for_user(self.u, 'home_tab'))

        views.activities(request)
        eq_('lets', Setting.get_for_user(self.u, 'home_tab'))
