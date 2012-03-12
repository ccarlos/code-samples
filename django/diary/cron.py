from datetime import datetime, timedelta

from django.conf import settings

import cronjobs

from diary.models import DiaryEntry


@cronjobs.register
def delete_old_drafts():
    """Delete diary drafts(not published) older than DIARY_GRACE_DAYS."""
    grace_days = datetime.now() - timedelta(days=settings.DIARY_GRACE_DAYS)
    drafts = DiaryEntry.objects.filter(is_draft=True, created__lte=grace_days)
    for entry in drafts:
        for pic in entry.pictures.all():
            pic.delete()
        entry.delete()
