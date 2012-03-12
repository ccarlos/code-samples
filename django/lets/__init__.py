from sowink.utils import choices_to_dict

CAT_FOOD = 10
CAT_SHOP = 20
CAT_ENTR = 30
CAT_DRIN = 40
CAT_SPOR = 50
CAT_MUSI = 60
CAT_VOLU = 70
CAT_EXPL = 80
CAT_ARTS = 90
CAT_OTHR = 100
CATEGORY_CHOICES = (
    (CAT_FOOD, 'Food'),
    (CAT_MUSI, 'Music'),
    (CAT_EXPL, 'Exploration'),
    (CAT_ENTR, 'Entertainment'),
    (CAT_VOLU, 'Volunteer'),
    (CAT_DRIN, 'Drinks'),
    (CAT_ARTS, 'Arts'),
    (CAT_SPOR, 'Sports'),
    (CAT_SHOP, 'Shopping'),
    (CAT_OTHR, 'Other'),
)
CATEGORIES = choices_to_dict(CATEGORY_CHOICES)

# Let icon locations.
LET_ICON = {
    CAT_FOOD: 'img/lets/cat-food.png',
    CAT_MUSI: 'img/lets/cat-musi.png',
    CAT_EXPL: 'img/lets/cat-expl.png',
    CAT_ENTR: 'img/lets/cat-entr.png',
    CAT_VOLU: 'img/lets/cat-volu.png',
    CAT_DRIN: 'img/lets/cat-drin.png',
    CAT_ARTS: 'img/lets/cat-arts.png',
    CAT_SPOR: 'img/lets/cat-spor.png',
    CAT_SHOP: 'img/lets/cat-shop.png',
    CAT_OTHR: 'img/lets/cat-othr.png',
}

LETS_PER_PAGE = 20


LET_ICON_CLASS = {
    CAT_FOOD: 'cat-food',
    CAT_MUSI: 'cat-musi',
    CAT_EXPL: 'cat-expl',
    CAT_ENTR: 'cat-entr',
    CAT_VOLU: 'cat-volu',
    CAT_DRIN: 'cat-drin',
    CAT_ARTS: 'cat-arts',
    CAT_SPOR: 'cat-spor',
    CAT_SHOP: 'cat-shop',
    CAT_OTHR: 'cat-othr',
}
