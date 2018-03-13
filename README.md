# RPP
Rashi Plus Plus - the new Hebrew scripting language

Interpreted, untyped, object-oriented and super cool.

<pre dir="rtl" align="right">
פלוט 'שלום עולם!'
</pre>

## Table of Contents

[Variables, comparison and math](#vars)

<a name="vars"/>
## Variables, comparison and math

<pre dir="rtl" align="right">
מ = 10
מ = מ / 100
פלוט מ // 0.1
מ = ((מ ** 2) * 300) % 40
פלוט מ // 3
פלוט מ שווהל 100 או 1 == 1 // true
פלוט מ גדולמ 70 וגם שקר // false
פלוט לא (מ קטןמ 0.34) // true
</pre>

### Operators

Any of the c-style operators can be used interchangeably with the hebrew keywords

| c-style | rpp |
| :------ | --: |
| == | שווהל |
| != | שונהמ |
| > | גדולמ |
| < | קטןמ |
| >= | --- |
| <= | --- |
| ! | לא |
| / | חלקי |
| * | כפול |
| % | --- |
| - | --- |
| + | --- |
| false | שקר |
| true | אמת |

- Power operator: `**` (python-style)

## Control flow

<pre dir="rtl" align="right">
מ = 1

כלעוד מ קטןמ 10:
	אם מ שווהל 2:
		פלוט 'שתיים'
	אחרת אם מ % 2 == 0:
		פלוט 'זוגי'
	אחרת:
		פלוט 'אי-זוגי'
	מ = מ + 1

// אי-זוגי
// שתיים
// אי-זוגי
// זוגי
// ...

לכל מ בתוך טווח(100):
	פלוט מ

// 0, 1, 2, ...
</pre>

### Keywords

| c-style | rpp |
| :------ | --: |
| if | אם |
| else | אחרת |
| while | שווהל |
| continue | המשך |
| break | שבור |

| python-style | rpp |
| :------ | --: |
| for | לכל |
| in | בתוך |

### Flexible syntax

All code blocks can be written in a one-line form or a multi-line indented block

<pre dir="rtl" align="right">
כלעוד אמת:
    פלוט 1

כלעוד אמת פלוט 1

אם שקר פלוט 1 אחרת אם אמת פלוט 2 אחרת:
    פלוט 3
// 2
</pre>

Supported code blocks:

- if, else if, else
- functions
- classes
- while loops
- for loops
- try, catch

## Functions

Functions in rpp are declared similarly to JavaScript - they can be declared by name or anonymously

<pre dir="rtl" align="right">
פיב = פעולה(מ):
	אם מ == 0 או מ == 1:
		החזר 1
	החזר פיב(מ - 1) + פיב(מ - 2)

פלוט_פיב = פעולה(מ) פלוט פיב(מ)

לכל מ בתוך טווח(5):
	פלוט_פיב(מ)

// 1, 1, 2, 3, 5, ...
</pre>

## Classes

Class declarations are declared similarly to functions (named or anonymous)

<pre dir="rtl" align="right">
מחלקה חישובים:
    פעולה כפל(א, ב):
        החזר א * ב

פלוט חישובים.כפל(4, 2)
// 8

חישובים2 = מחלקה:
    פעולה ריבוע(מ):
        החזר מ ** 2
        
פלוט חישובים2.ריבוע(3)
// 9
</pre>

### Magic methods (dunders)

Similarly to Python, rpp classes can implement "magic" methods that run in certain situations

<pre dir="rtl" align="right">
מחלקה סניף:
    פעולה __התחל__(שם):
        אני.שם = שם
        אני.עובדים = רשימה()
    
    פעולה הוסף_עובד(שם_עובד):
        אני.עובדים.הוסף(שם_עובד)
    
    פעולה הסר_עובד_אחרון():
        אני.עובדים.הסר(אני.עובדים.גודל() - 1)
    
    פעולה __טקסט__():
        החזר אני.שם + ': ' + טקסט(אני.עובדים)

סניף_מרכזי = סניף('תל אביב')
סניף_מרכזי.הוסף_עובד('דניאל')

פלוט סניף_מרכזי

// תל אביב: [דניאל]
</pre>

| python-style | rpp |
| :----------- | --: |
| `__init__` | `__התחל__` |
| `__str__` | `__טקסט__` |
| `__getitem__` | `__קח__` |
| `__setitem__` | `__שים__` |
| `__next__` | `__הבא__` |
| `__iterator__` | `__איטרטור__` |

## Built-ins

As all great programming languages, rpp is equipped with some useful built-ins

### List

#### constructor

`רשימה(...)`

Creates list with elements from arguments

#### Get and Set

Available with classic [] operator

#### Size

`א.גודל()`
