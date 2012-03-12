from django.contrib.auth.models import User
from django.core.cache import cache
from django.core.exceptions import PermissionDenied
from django.core.urlresolvers import reverse
from django.http import HttpResponseRedirect
from django.shortcuts import get_object_or_404
from django.views.decorators.http import require_POST

import jingo

from access.decorators import login_required
from cachesw import invalidate_count
import lets as lcs
from lets.forms import LetsForm
from lets.models import Lets, LetsVote, CATEGORY_CHOICES
from lets.utils import clean_category, get_relevant_lets
from mojo.utils import (process_simple_action, process_mojo_action,
    process_mojo_action_down)
from photos.utils import profile_picture_url as pp_url
from rewards.utils import update_rewards
from sowink.helpers import timesince
from sowink.utils import (JSONResponse, get_next_url, paginate, raw_to_plain,
    js_datetime)
import users as ucs
from users.utils import update_progress


@require_POST
@login_required
# TODO: ratelimit to prevent spamming
def new(request):
    """Create a new Lets update."""

    form = LetsForm(creator=request.user, data=request.POST)
    form_valid = form.is_valid()
    if form_valid:
        let = form.save()
        u = request.user
        let_entries = list(u.lets.all()[:3])
        u.lets.invalidate(*let_entries)
        update_progress(u.profile, action='lets')
        update_rewards(u, 'lets')
        process_mojo_action(u, 'lets')

    if request.is_ajax():
        data = {'is_valid': form_valid,
                'errors': form.errors,
                'msg': 'Saved'}
        if form_valid:
            data['user'] = {
                'profile_url': reverse('users.profile',
                                        args=[request.user.username]),
                'picture_url': pp_url(request.user),
                'display_name': request.user.first_name,
            }
            data['letobj'] = {
                'content': "Let's... " + raw_to_plain(let.content),
                'category_url': let.get_category_url(),
                'category_class': let.get_category_class(),
                'category_display': let.get_category_display(),
                'category_img': let.get_category_img(large=True),
                'js_date': js_datetime(let.created),
                'timesince': timesince(let.created),
                'delete_url': reverse('lets.delete', args=[let.pk]),
            }
        return JSONResponse(data)

    next_url = reverse('lets.wall', args=[request.user.username])
    return HttpResponseRedirect(next_url)


@require_POST
@login_required
def delete(request, lets_id):
    """Delete a Lets object, only if logged in user is the creator."""

    lets = get_object_or_404(Lets, pk=lets_id)
    if lets.creator != request.user:
        raise PermissionDenied

    lets.delete()

    # Coins
    profile = request.user.profile
    user_key = Lets.u_cache_key(request.user)
    lets_key = cache.get(user_key)
    if lets_key:
        if lets_key > 0:
            # No 250 -
            cache.decr(user_key)

        if lets_key <= 0:
            # Yes 250 -
            profile.update(coins=profile.coins -
                           ucs.ACTIONS_COINS['lets']['daily'])
            cache.delete(user_key)
    else:
        # yes 250 -
        profile.update(coins=profile.coins -
                       ucs.ACTIONS_COINS['lets']['daily'])
    update_progress(profile, action='lets')
    update_rewards(request.user, 'lets')
    process_mojo_action_down(request.user, 'lets')

    # TODO: return progress_data with ajax request. Modify js on lets side to
    #   update profile completion widget
    next_url = reverse('lets.wall', args=[request.user.username])
    if request.is_ajax():
        return JSONResponse({'is_valid': True,
                             'msg': 'Deleted'})
    return HttpResponseRedirect(next_url)


@login_required(only_active=False)
def activities(request, category=None, home_tab=None):
    search_lets = get_relevant_lets(
        request.user, page=request.GET.get('page'),
        category=request.GET.get('category'))
    pages = paginate(request, search_lets['ids'], lcs.LETS_PER_PAGE)
    data = {'lets': search_lets['results'], 'catg': CATEGORY_CHOICES,
            'pages': pages, 'profile': request.user.profile}

    if not home_tab:
        from users.models import Setting
        home_tab = Setting.get_for_user(request.user, 'home_tab')
        if home_tab != 'lets':
            from users.forms import SettingsForm
            form = SettingsForm(data={'home_tab': 'lets'})
            form.is_valid()
            form.save_for_user(request.user)

    return jingo.render(request, 'lets/activities.html', data)


@login_required(only_active=False)
def wall(request, username):
    user = get_object_or_404(User, username=username)
    lets = user.lets.all()
    category = clean_category(request.GET.get('category'))
    if category:
        lets = lets.filter(category=category)
    pages = paginate(request, lets, lcs.LETS_PER_PAGE)
    data = {'pages': pages, 'catg': CATEGORY_CHOICES, 'profile': user.profile}
    return jingo.render(request, 'lets/wall.html', data)


@require_POST
@login_required
def like_lets(request, lets_id):
    lets = get_object_or_404(Lets, pk=lets_id)

    if lets.creator == request.user:
        raise PermissionDenied

    # If already liked, just redirect back to profile.
    my_vote = lets.let_votes_received.filter(creator=request.user)
    if not my_vote:
        my_vote = LetsVote.objects.create(creator=request.user, lets_obj=lets)
        invalidate_count(lets.let_votes_received.all())
        lets.let_votes_received.invalidate(my_vote)
        process_simple_action(lets.creator, request.user, 'lets_like')

    if request.is_ajax():
        return JSONResponse({'is_valid': True})
    next_url = get_next_url(request)
    return HttpResponseRedirect(next_url)
