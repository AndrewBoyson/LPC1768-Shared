//Override the base font size 1vw = 1% width so for a width of 600px 4vw equates to 24px
"                            * { font-size:4vw;  } \r\n"
"@media (min-width: 600px) { * { font-size:24px; } }\r\n"

//Override the left margin to fit the nav bar and make the line spacing smaller
"body              { margin-left:6em; }\r\n"

//Overide the line container to match
".line             { width:17em; }\r\n"

//Specify the tab access
".tab-shortcut       { position:absolute; top:-1000em;}\r\n"
".tab-shortcut:focus { position:fixed; top:0; left:0; z-index:999; padding:0.5em; background-color:darkblue; color:white; }\r\n"

//Specify the nav bar itself
"nav ul            { list-style-type:none; margin:0; padding:0; overflow:hidden; position:fixed; top:0; left:0; width:5em; font-size:1.2em; }\r\n"
"nav ul li         { background:lightskyblue; padding:0; border-style:none; margin:0.2em; }\r\n"
"nav ul li.this    { background:coral; }\r\n"
"nav ul li a       { display:block; color:black; border:0;margin:0; padding:0.4em; }\r\n"
"nav ul li a:hover { color:darkred; }\r\n"
