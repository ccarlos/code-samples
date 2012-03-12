import datetime
import json

from django.conf import settings
from django.contrib.auth.models import User
from django.core.exceptions import PermissionDenied
from django.core.urlresolvers import reverse
from django.http import HttpResponseRedirect, HttpResponse
from django.shortcuts import get_object_or_404
from django.views.decorators.http import require_http_methods, require_POST

from commonware.decorators import xframe_sameorigin
import jingo

from access.decorators import login_required
from cachesw import invalidate_count
from diary.events import NewCommentEvent
from diary.forms import DiaryForm, CommentForm
from diary.models import DiaryEntry, Comment, DiaryPicture
from diary.utils import (get_kwargs_for_diary_id, get_diary_calendar,
                         clean_diary_date, diary_today_exists,
                         draft_today_exists, create_diary_pic)
from mojo.utils import process_mojo_action, process_mojo_action_down
from notify.utils import process_diary
from photos.forms import ImageForm
from rewards.utils import update_rewards
from sowink.helpers import urlparams
from sowink.utils import smart_int, JSONResponse
import users as ucs
from users.utils import get_users_and_visits


ENTRY_DELETED = 'Successfully deleted the entry.'
MSG_INVALID_PIC = 'Invalid or no picture received.'
MSG_PIC_LIMIT = 'You have reached the limit for this diary entry.'
MSG_NO_ENTRY = 'No entry found.'
MSG_PIC_SUCCESS = 'Successfully uploaded picture.'
MSG_PIC_PERM = 'You do not have permission to upload to this entry.'
MSG_PIC_DEL_SUCCESS = 'Successfully deleted picture.'
MSG_PIC_DEL_PERM = 'You do not have permission to delete this picture.'
JSResponse = lambda data, **kw: HttpResponse(json.dumps(data), **kw)
MIN_COMMENT_LENGTH_MOJO = 100


@login_required(only_active=False)
def list_personal(request, username, year=None, month=None):
    """Lists the diaries belonging to username.

    If the username is the name of the logged in user,
    it will display drafts and private.
    If it is another user's page it only displays non-draft,
    non-private entries.

    """
    if username == 'me':
        return HttpResponseRedirect(
            reverse('diary.list_all', args=[request.user.username]))

    user = get_object_or_404(User, username=username)

    if user != request.user and not request.user.is_active:
        next_url = urlparams(reverse('users.inactive'), next=request.path)
        return HttpResponseRedirect(next_url)

    get_users_and_visits(user, request.user)

    c_year, c_month = clean_diary_date(year, month)
    today = datetime.datetime.today()
    current_year_month = True if (c_month == today.month and
                                  c_year == today.year) else False
    if year and month:
        entries = user.diaries.get_for_month(visitor=request.user,
                                             creator=user,
                                             year=c_year, month=c_month)
    else:
        entries = user.diaries.get_all(visitor=request.user,
                                       creator=user)

    entry_today_exists = diary_today_exists(entries)
    data = {'entries': entries, 'creator': user,
            'calendar_data': get_diary_calendar(entries, c_year, c_month),
            'entry_today_exists': entry_today_exists}
    # Do we display a new diary entry form or draft form?
    if (request.user == user and not entry_today_exists and
        current_year_month):
        if draft_today_exists(entries):
            form = DiaryForm(creator=entries[0].creator, instance=entries[0])
            data['draft_exists'] = True
        else:
            form = DiaryForm(creator=request.user)
        data['diary_form'] = form

    return jingo.render(request, 'diary/list.html', data)


@require_http_methods(['GET', 'POST'])
@login_required(only_active=False)
def new(request):
    """Create a new diary entry for today."""
    diaries = request.user.diaries.filter(created_day=datetime.date.today())
    # If they already have a diary for today, redirect them to edit
    if diaries:
        return HttpResponseRedirect(reverse('diary.edit',
                                            args=[diaries[0].pk]))
    if request.method == 'POST':
        data = request.POST
        if request.is_ajax():
            data = data.copy()
            data['is_draft'] = True
        form = DiaryForm(creator=request.user, data=data)
        if form.is_valid():
            entry = form.save()
            u = request.user
            entries = list(u.diaries.get_all(visitor=u, creator=u)[:10])
            u.diaries.invalidate(*entries)
            # Register user to receive email when a comment is posted.
            NewCommentEvent.notify(request.user, entry)
            # Send news feed item to creator's favorites.
            if not entry.is_draft:
                process_diary(entry)
                u.profile.update(
                    coins=u.profile.coins +
                        ucs.ACTIONS_COINS['diary']['daily'])
                update_rewards(request.user, 'diary')
                process_mojo_action(request.user, 'diary_entry')
            if request.is_ajax():
                data = {'is_valid': True,
                        'is_new': True,
                        'url': reverse('diary.edit', args=[entry.pk]),
                        'timestamp': entry.updated.strftime("%s")}
                return JSONResponse(data)
            return HttpResponseRedirect(reverse('diary.list_all',
                                        args=[request.user.username]))
    else:   # request.method == 'GET'
        form = DiaryForm(creator=request.user)
    c_year, c_month = clean_diary_date()
    entries = request.user.diaries.get_for_month(visitor=request.user,
                                                 creator=request.user,
                                                 year=c_year, month=c_month)
    calendar_data = get_diary_calendar(entries, c_year, c_month)
    data = {'form': form, 'creator': request.user,
            'calendar_data': calendar_data}
    return jingo.render(request, 'diary/new.html', data)


@login_required(only_active=False)
@require_http_methods(['GET', 'POST'])
def edit(request, diary_id):
    """Edit an existing diary entry.

    Checks and adds errors for setting a non-draft diary to a draft diary.

    """
    entry = get_object_or_404(DiaryEntry, pk=diary_id)
    if entry.creator != request.user:
        raise PermissionDenied
    was_draft = entry.is_draft
    if request.method == 'POST':
        data = request.POST
        if request.is_ajax():
            data = data.copy()
            data['is_draft'] = True
        form = DiaryForm(creator=entry.creator, data=data,
                         instance=entry)
        if form.is_valid():
            entry = form.save()
            u = request.user
            entries = list(u.diaries.get_all(visitor=u, creator=u)[:10])
            u.diaries.invalidate(*entries)
            # Publishing a diary entry. Notify favorites.
            if was_draft and not entry.is_draft:
                process_diary(entry)
                u.profile.update(
                    coins=u.profile.coins +
                        ucs.ACTIONS_COINS['diary']['daily'])
                update_rewards(request.user, 'diary')
                process_mojo_action(request.user, 'diary_entry')
            if request.is_ajax():
                data = {'is_valid': True,
                        'timestamp': entry.updated.strftime("%s")}
                return JSONResponse(data)
            return HttpResponseRedirect(reverse(
                'diary.single', kwargs=get_kwargs_for_diary_id(entry.pk)))
    else:
        form = DiaryForm(creator=entry.creator, instance=entry)
    c_year, c_month = clean_diary_date(entry.created_day.year,
                                       entry.created_day.month)
    entries = request.user.diaries.get_for_month(visitor=request.user,
                                                 creator=request.user,
                                                 year=c_year, month=c_month)
    calendar_data = get_diary_calendar(entries, c_year, c_month)
    data = {'form': form, 'creator': request.user, 'entry': entry,
            'calendar_data': calendar_data}
    return jingo.render(request, 'diary/edit.html', data)


@login_required
@require_POST
def reply(request, diary_id):
    """Adds a comment to a diary entry where pk=diary_id."""
    entry = get_object_or_404(DiaryEntry, pk=diary_id)
    if entry.creator != request.user and (entry.is_private or entry.is_draft):
        raise PermissionDenied

    form = CommentForm(creator=request.user, diary=entry,
                       data=request.POST)
    if form.is_valid():
        comment = form.save()
        if len(comment.text) >= MIN_COMMENT_LENGTH_MOJO:
            process_mojo_action(request.user, 'diary_comment')
        invalidate_count(entry.comments.all())
        NewCommentEvent.notify(request.user, entry)
        # Send notifications to diary comment watchers.
        NewCommentEvent(comment).fire(exclude=request.user)
        return HttpResponseRedirect(entry.get_absolute_url())

    return single(request, username=entry.creator.username,
                  year=entry.created_day.year, month=entry.created_day.month,
                  day=entry.created_day.day, comment_form=form)


@login_required(only_active=False)
def single(request, username, year, month, day, comment_form=None):
    """View a diary page of the user with username.

    Finds a diary entry by using the year month and day of it.
    If those fields are not specified, uses the current date.

    """
    user = get_object_or_404(User, username=username)

    if user != request.user and not request.user.is_active:
        next_url = urlparams(reverse('users.inactive'), next=request.path)
        return HttpResponseRedirect(next_url)

    get_users_and_visits(user, request.user)

    c_year, c_month = clean_diary_date(year, month)
    c_day = smart_int(day)
    entries = user.diaries.get_for_month(visitor=request.user,
                                         creator=user,
                                         year=c_year, month=c_month)
    calendar_data = get_diary_calendar(entries, c_year, c_month)
    if c_day not in calendar_data['days_with_entries']:
        url = reverse('diary.list_all', args=[user.username])
        return HttpResponseRedirect(url)
    entry = calendar_data['days_with_entries'][c_day]

    if not comment_form:
        comment_form = CommentForm(creator=request.user, diary=entry)

    data = {'entry': entry,
            'calendar_data': calendar_data,
            'comments': entry.comments.order_by('created'),
            'diary': DiaryForm(creator=entry.creator, instance=entry),
            'comment_form': comment_form, 'creator': user}
    return jingo.render(request, 'diary/single.html', data)


@require_POST
@login_required(only_active=False)
def delete(request, diary_id):
    """Deletes a diary, only if logged in user is the creator."""
    entry = get_object_or_404(DiaryEntry, pk=diary_id)
    if entry.creator != request.user:
        raise PermissionDenied

    for pic in entry.pictures.all():
        pic.image.delete()
        pic.thumbnail.delete()
    entry.delete()
    request.user.profile.update(
        coins=request.user.profile.coins - ucs.ACTIONS_COINS['diary']['daily'])
    update_rewards(request.user, 'diary')
    process_mojo_action_down(request.user, 'diary_entry')
    url = reverse('diary.list_all', args=[request.user.username])
    if request.is_ajax():
        return JSONResponse({'is_valid': True, 'success': ENTRY_DELETED,
                             'url': url})
    return HttpResponseRedirect(url)


@require_POST
@login_required(only_active=False)
def delete_comment(request, comment_id):
    """Deletes a comment from a diary.

    Only deletes the comment if the logged in user is the creator of the
    comment or of the diary.

    """
    comment = get_object_or_404(Comment, pk=comment_id)
    entry = comment.diary
    comment_creator = comment.creator
    if comment.creator != request.user and entry.creator != request.user:
        raise PermissionDenied
    comment.delete()
    process_mojo_action_down(comment_creator, 'diary_comment')
    # Does comment_creator have a comment in the diary entry?
    # If not, stop notifying them.
    if not entry.comments.filter(creator=comment_creator).exists():
        NewCommentEvent.stop_notifying(comment_creator, entry)
    return HttpResponseRedirect(reverse(
        'diary.single', kwargs=get_kwargs_for_diary_id(entry.pk)))


@require_POST
@login_required(only_active=False)
@xframe_sameorigin
def upload_pic(request, entry_id=None):
    """Attaches picture to the given diary entry.

    Defaults to today if no entry provided.
    Creates a new entry if one does not already exist.

    """
    entry_created = False
    if entry_id:
        try:
            entry = DiaryEntry.objects.get(pk=entry_id)
        except DiaryEntry.DoesNotExist:
            return JSResponse({'is_valid': False,
                               'errors': {'__all__': [MSG_NO_ENTRY]}},
                              status=404)
    else:
        try:
            entry = DiaryEntry.uncached.get(created_day=datetime.date.today(),
                                            creator=request.user)
        except DiaryEntry.DoesNotExist:
            entry = DiaryEntry.objects.create(
                creator=request.user, is_draft=True)
            entry_created = True

    if entry.creator != request.user:
        return JSResponse({'is_valid': False,
                           'errors': {'__all__': [MSG_PIC_PERM]}},
                          status=403)

    form = ImageForm(request.POST, request.FILES)
    if not form.is_valid():
        return JSResponse({'is_valid': False,
                           'errors': {'__all__': [MSG_INVALID_PIC]}},
                          status=404)

    # Check the number of pictures for this entry is not too high.
    if entry.pictures.count() >= settings.DIARY_PICS_PER_ENTRY:
        return JSResponse({'is_valid': False,
                           'errors': {'__all__': [MSG_PIC_LIMIT]}},
                          status=400)

    up_file = request.FILES['image']
    diary_pic = create_diary_pic(up_file, entry)
    all_pics = entry.pictures.all()
    entry.pictures.invalidate(*list(all_pics))
    invalidate_count(all_pics)

    delete_url = reverse('diary.delete_pic', args=[diary_pic.id])
    data = ({'name': up_file.name, 'url': diary_pic.image.url,
             'width': settings.DIARY_THUMBNAIL_SIZE[0],
             'height': settings.DIARY_THUMBNAIL_SIZE[1],
             'entry_id': diary_pic.entry.pk, 'delete_url': delete_url,
             'thumbnail_url': diary_pic.image.url,
             'url': diary_pic.image.url})
    if entry_created:
        data['entry_edit_url'] = reverse('diary.edit', args=[entry.pk])
    return JSResponse({'is_valid': True, 'success': MSG_PIC_SUCCESS,
                       'picture': data})


@require_POST
@login_required(only_active=False)
@xframe_sameorigin
def delete_pic(request, image_id):
    """Delete an image given its object id."""
    try:
        diary_pic = DiaryPicture.objects.get(pk=image_id)
    except DiaryPicture.DoesNotExist:
        return JSResponse({'is_valid': False,
                           'status': 404,
                           'errors': {'__all__': [MSG_INVALID_PIC]}},
                          status=404)

    if diary_pic.entry.creator != request.user:
        return JSResponse({'is_valid': False,
                           'status': 403,
                           'errors': {'__all__': [MSG_PIC_DEL_PERM]}},
                          status=403)

    diary_pic.image.delete()
    diary_pic.thumbnail.delete()
    diary_pic.delete()

    return JSResponse({'is_valid': True, 'success': MSG_PIC_DEL_SUCCESS})
