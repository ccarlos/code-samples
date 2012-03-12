from django.conf.urls.defaults import url, patterns


urlpatterns = patterns('diary.views',
    url(r'^user/(?P<username>[-\w_\.]+)/(?P<year>\d+)/(?P<month>\d+)/'
        r'(?P<day>\d+)', 'single', name='diary.single'),
    url(r'^user/(?P<username>[-\w_\.]+)/(?P<year>\d+)/(?P<month>\d+)',
        'list_personal', name='diary.list_month'),
    url(r'^user/(?P<username>[-\w_\.]+)',
        'list_personal', name='diary.list_all'),

    url(r'^entry/(?P<diary_id>\d+)/reply$', 'reply', name='diary.reply'),
    url(r'^entry/(?P<diary_id>\d+)/delete$', 'delete', name='diary.delete'),
    url(r'^entry/(?P<diary_id>\d+)/edit$', 'edit', name='diary.edit'),
    url(r'^new$', 'new', name='diary.new'),
    url(r'^delete_comment/(?P<comment_id>\d+)$',
        'delete_comment', name='diary.delete_comment'),

    url(r'^picture/upload/(?P<entry_id>\d+)?$', 'upload_pic',
        name='diary.upload_pic'),
    url(r'^image/delete/(?P<image_id>\d+)$',
        'delete_pic', name='diary.delete_pic'),
)
