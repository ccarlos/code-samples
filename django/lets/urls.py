from django.conf.urls.defaults import url, patterns


urlpatterns = patterns('lets.views',
    url(r'^act/(?P<lets_id>\d+)/delete$', 'delete', name='lets.delete'),
    url(r'^act/(?P<lets_id>\d+)/like$', 'like_lets', name='lets.like'),

    url(r'^new$', 'new', name='lets.new'),

    # NOTE: change welcome notice when you change lets
    url(r'^activities$', 'activities', name='lets'),
    url(r'^wall/user/(?P<username>[-\w_\.]+)$', 'wall', name='lets.wall'),
)
