import lets as lcs
from lets.models import Lets
import search as scs
from search.clients import LetsClient, SearchError
from search.utils import get_blocked_uids
from sowink.utils import smart_int
from users.models import LOOKING_SEX_3


def get_relevant_lets(user, category=None, page=1, per_page=lcs.LETS_PER_PAGE):
    lc = LetsClient()
    page = smart_int(page, 1)

    # Continue with search
    filters = []

    category = clean_category(category)
    if category:
        filters.append({
            'filter': 'category',
            'value': [category],
        })

    gender = user.profile.looking_sex
    if gender == LOOKING_SEX_3:
        gender = None

    if gender:
        filters.append({
            'filter': 'gender',
            'value': [gender],
        })

    filters.append({
        'filter': 'country',
        'value': [lc.str_to_int('US')],
    })
    filters.append({
        'filter': 'state',
        'value': [lc.str_to_int('CA')],
    })
    filters.append({
        'filter': 'creator_id',
        'value': get_blocked_uids(user),
        'exclude': True,
    })

    try:
        lc.set_sort_mode(scs.S_LETS_SPH[0], scs.S_LETS_SPH[1])
        lets = lc.query('', filters)
    except SearchError:
        lets = Lets.objects.exclude(creator=user)
        if category:
            lets = lets.filter(category=category)
        return {'results': lets, 'ids': lets}
    lets_ids = []
    offset = (page - 1) * per_page
    for i in range(offset, offset + lcs.LETS_PER_PAGE):
        try:
            lets_ids.append(lets[i]['id'])
        except IndexError:
            break

    results = Lets.objects.select_related('creator').filter(pk__in=lets_ids)
    return {'results': sorted(results, key=lambda r: lets_ids.index(r.pk)),
            'ids': lets}


def clean_category(category):
    category = smart_int(category)
    if category not in lcs.CATEGORIES.keys():
        return None
    return category
