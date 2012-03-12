from django.contrib import admin

from lets import models


class LetsAdmin(admin.ModelAdmin):
    raw_id_fields = ['creator']


class LetsVoteAdmin(admin.ModelAdmin):
    raw_id_fields = ['creator']


admin.site.register(models.Lets, LetsAdmin)
admin.site.register(models.LetsVote, LetsAdmin)
admin.site.register(models.LetsMap, admin.ModelAdmin)
