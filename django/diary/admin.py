from django import forms
from django.conf import settings
from django.contrib import admin

from diary.models import Comment, DiaryEntry, DiaryPicture

from sowink.form_fields import StrippedCharField


class DiaryEntryAdmin(admin.ModelAdmin):
    fields = ['created', 'created_day', 'text', 'updated', 'is_draft',
              'is_private']
    list_display = ['creator', 'created_day', 'is_draft']
    list_filter = ['is_draft', 'is_private', 'updated', 'created']
    readonly_fields = ['creator']
    search_fields = ['creator__username']

    def save_model(self, request, obj, form, change):
        if not obj.pk:
            obj.creator = request.user
        obj.save()


class DiaryPictureForm(forms.ModelForm):
    image = StrippedCharField(label='Image:',
                              max_length=settings.MAX_FILEPATH_LENGTH)
    thumbnail = StrippedCharField(label='Thumbnail:',
                              max_length=settings.MAX_FILEPATH_LENGTH)

    class Meta(object):
        model = DiaryPicture


class DiaryPictureAdmin(admin.ModelAdmin):
    form = DiaryPictureForm
    list_display = ['entry', 'created']
    raw_id_fields = ['entry']


class CommentAdmin(admin.ModelAdmin):
    fields = ['created', 'text', 'diary']
    list_display = ['creator', 'created', 'diary']
    list_filter = ['created', 'diary']
    readonly_fields = ['creator', 'diary']
    search_fields = ['creator__username']


admin.site.register(DiaryEntry, DiaryEntryAdmin)
admin.site.register(DiaryPicture, DiaryPictureAdmin)
admin.site.register(Comment, CommentAdmin)
