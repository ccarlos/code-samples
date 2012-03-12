from collections import defaultdict
from datetime import datetime

import cronjobs

from lets.models import LetsVote
from notify import mail
from users.models import EmailNotification


@cronjobs.register
def lets_like_digest():
    # Map between: Lets creator => LetsVotes objects.
    d = defaultdict(list)
    for like in likes_today():
        d[like.lets_obj.creator].append(like)

    # Find users that want to receive email notifications.
    prefs_on = [en.user for en in EmailNotification.objects.select_related(
        'user').filter(user__in=d.keys()) if en.lets]

    for creator, likes in d.iteritems():
        if creator in prefs_on:
            send_lets_digest_email(creator, likes)


def likes_today():
    today = datetime.today()
    return LetsVote.objects.filter(created__year=today.year,
                                   created__month=today.month,
                                   created__day=today.day)


def send_lets_digest_email(creator, likes):
    # Break up likes: Map: Lets => [users voted].
    d = defaultdict(list)
    all_u = []
    for li in likes:
        d[li.lets_obj].append(li.creator)
        all_u.append(li.creator.username)

    mail.LetsDigestEmail(to=[creator.email], key=True,
        reason='lets', lets_map=d, all_usernames=all_u).send(delay=False)
