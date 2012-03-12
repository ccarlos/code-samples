from celery.decorators import task
from tidings.events import InstanceEvent

from diary.models import DiaryEntry
from notify import mail
from users.models import EmailNotification


class NewCommentEvent(InstanceEvent):
    """An event which fires when a DiaryEntry receives a reply

    Firing this also notifies watchers of the containing Diary.

    """
    event_type = 'reply'
    content_type = DiaryEntry

    def __init__(self, comment):
        super(NewCommentEvent, self).__init__(comment.diary)
        # Need to store the comment and entry for _mails
        self.comment = comment
        self.entry = comment.diary

    @task
    def _fire_task(self, exclude=None):
        """Build and send the emails as a celery task."""
        for m in self._mails(self._users_watching(exclude=exclude)):
            m.send(delay=False)

    def _mails(self, users_and_watches):
        users = [u for u, _ in users_and_watches]
        prefs_on = [en for en in EmailNotification.objects.filter(
            user__in=users) if en.diary]
        for en in prefs_on:
            yield mail.DiaryCommentEmail(
                to=[en.user.email], key=True, reason='diary-comment',
                data={'comment': self.comment})
