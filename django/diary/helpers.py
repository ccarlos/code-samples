from django.core.urlresolvers import reverse

from jingo import register


@register.function
def diary_entry_url(entry):
    """Returns a diary entry's URL, by creation date."""
    return reverse('diary.single',
                   args=[entry.creator.username, entry.created_day.year,
                         entry.created_day.month])
