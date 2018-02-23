# RPP
Rashi Plus Plus - the new Hebrew scripting language

Interpreted, untyped, object-oriented and super cool

<pre dir="rtl" align="right">
פלוט 'שלום עולם!'
</pre>

## Variables, comparison and logic

<pre dir="rtl" align="right">
מספר_חשוב = 23

אם מספר_חשוב גדולמ 20:
    אם מספר_חשוב שווהל 23 פלוט 'מדהים!' אחרת:
        פלוט 'שונה'
    // output: מדהים

אחרת אם אמת או שקר פלוט 'סבבה'
// output: סבבה
</pre>

## Loops and functions

<pre dir="rtl" align="right">
אקו = פעולה(חזרות):
    הודעה = קלוט('')
    
    מספר = 0
    כלעוד מספר קטןמ חזרות:
        פלוט הודעה
        מספר = מספר + 1

// חזרה אחת
אקו(1)
// שתי חזרות
אקו(2)
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


