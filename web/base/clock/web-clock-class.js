//Clock class
'use strict';

class Clock
{
    constructor()
    {
        this.leapEnable  = false;
        this.leapForward = false;
        this.leapMonth   = 0;
        this.leapYear    = 0;
        this.leaps       = 0;
        this.ms          = 0;
    }
    
    set months1970(value)
    {
        this.leapMonth   =  value                   % 12;
        this.leapYear    = (value - this.leapMonth) / 12;
        this.leapMonth  += 1;
        this.leapYear   += 1970;
    }
    get months1970()
    {
        if (this.leapYear  <= 0) return 0;
        if (this.leapMonth <= 0) return 0;
        return (this.leapYear - 1970) * 12 + this.leapMonth - 1;
    }
    
    formatNumbers00(i)
    {
       if (i < 10) return '0' + i;
       return i;
    }
    formatDayOfWeek(wday)
    {
        switch(wday)
        {
            case  0: return 'Sun';
            case  1: return 'Mon';
            case  2: return 'Tue';
            case  3: return 'Wed';
            case  4: return 'Thu';
            case  5: return 'Fri';
            case  6: return 'Sat';
            default: return '---';
        }
    }
    adjustLeap(baseMs)
    {
        if (this.ms == 0) return; //Don't attempt to adjust an invalid time
        
        if (!this.leapEnable) return; // Adjustment disabled
        
        //Get the calander date and time from the ms
        let now       = this.ms + baseMs;
        let leapStart = Date.UTC(this.leapYear, this.leapMonth - 1, 1, 0, 0, this.leapForward ? 0: -1);
        
        if (now < leapStart) return; //Do nothing until reached the leap start
        
        if (this.leapForward) { this.ms -= 1000; this.leaps += 1; } //repeat 59
        else                  { this.ms += 1000; this.leaps -= 1; } //skip   59
        
        this.leapEnable = false;
    }
    displayTime(baseMs)
    {
        if (this.ms == 0) return; //Don't attempt to display an invalid time
        
        //Get the calander date and time from the ms
        let now = new Date(this.ms + baseMs);
        let   y = now.getUTCFullYear();
        let   n = now.getUTCMonth   () + 1;
        let   d = now.getUTCDate    ();
        let   w = now.getUTCDay     (); // 0 == Sunday
        let   h = now.getUTCHours   ();
        let   m = now.getUTCMinutes ();
        let   s = now.getUTCSeconds ();
        
        //Format time
        n = this.formatNumbers00(n);
        d = this.formatNumbers00(d);
        h = this.formatNumbers00(h);
        m = this.formatNumbers00(m);
        s = this.formatNumbers00(s);
        w = this.formatDayOfWeek(w);
        
        //Display time
        let elem;
        elem = document.getElementById('ajax-date-utc');
        if (elem) elem.textContent = y + '-' + n + '-' + d + ' ' + w + ' ' + h + ':' + m + ':' + s + ' TAI-UTC=' + this.leaps;
    
        elem = document.getElementById('ajax-date-pc');
        let options = 
        {
            year:         'numeric',
            month:        'short',
            day:          '2-digit',
            weekday:      'short',
            hour:         '2-digit',
            minute:       '2-digit',
            second:       '2-digit',
            timeZoneName: 'short'
        };
        if (elem) elem.textContent = now.toLocaleString(undefined, options);
    }
}
