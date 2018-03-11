# RPP
Rashi Plus Plus - the new Hebrew scripting language

Interpreted, untyped, object-oriented and super cool.

<pre dir="rtl" align="right">
פלוט 'שלום עולם!'
</pre>

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

| python-style | rpp |
| :------ | --: |
| for | לכל |
| in | בתוך |

## Functions

<pre dir="rtl" align="right">
פיב = פעולה(מ):
	אם מ == 0 או מ == 1:
		החזר 1
	החזר פיב(מ - 1) + פיב(מ - 2)

לכל מ בתוך טווח(5):
	פלוט פיב(מ)

// 1, 1, 2, 3, 5, ...
</pre>

## Classes

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
        פלוט אני.שם
        פלוט אני.עובדים

סניף_מרכזי = סניף('תל אביב')
סניף_מרכזי.הוסף_עובד('דניאל')

פלוט סניף_מרכזי

// output: תל אביב
// output: דניאל 
</pre>


