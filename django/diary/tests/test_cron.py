from datetime import datetime, timedelta

from django.conf import settings
from django.core.files import File

from nose.tools import eq_

from diary.cron import delete_old_drafts
from diary.models import DiaryEntry, DiaryPicture
from sowink.tests import TestCase
from users.tests import get_user


class DeleteDraftsCronTests(TestCase):
    def setUp(self):
        super(DeleteDraftsCronTests, self).setUp()
        self.c = get_user(username='ccarlos')
        grace_days = (datetime.now() -
                      timedelta(days=settings.DIARY_GRACE_DAYS + 1))

        self.draft = DiaryEntry(creator=self.c, created=grace_days,
                               is_draft=True)
        self.draft.save()
        self.draft_pk = self.draft.pk
        draft_pic = DiaryPicture(entry=self.draft)
        draft_pic2 = DiaryPicture(entry=self.draft)

        self.entry = DiaryEntry(creator=self.c, is_draft=False)
        self.entry.save()
        self.entry_pk = self.entry.pk
        entry_pic = DiaryPicture(entry=self.entry)
        entry_pic2 = DiaryPicture(entry=self.entry)

        with open('apps/diary/tests/media/django.jpg') as f:
            img = File(f)
            draft_pic.image.save('cowboy', img, save=False)
            draft_pic.thumbnail.save('cowboy', img, save=False)

            entry_pic.image.save('cowboy_new', img, save=False)
            entry_pic.thumbnail.save('cowboy_new', img, save=False)

            draft_pic.save()
            entry_pic.save()

        with open('apps/diary/tests/media/hawk.jpg') as f:
            img = File(f)
            draft_pic2.image.save('hawk', img, save=False)
            draft_pic2.thumbnail.save('hawk', img, save=False)

            entry_pic2.image.save('hawk_new', img, save=False)
            entry_pic2.thumbnail.save('hawk_new', img, save=False)

            draft_pic2.save()
            entry_pic2.save()

    def tearDown(self):
        super(DeleteDraftsCronTests, self).tearDown()
        DiaryPicture.objects.all().delete()

    def test_cron_old_drafts(self):
        delete_old_drafts()

        # Assert cron function deleted draft.
        fn = lambda: DiaryEntry.objects.get(pk=self.draft_pk)
        self.assertRaises(DiaryEntry.DoesNotExist, fn)

        # Assert cron function did not delete self.entry or its pictures.
        entry = DiaryEntry.objects.get(pk=self.entry_pk)
        eq_(2, entry.pictures.count())
