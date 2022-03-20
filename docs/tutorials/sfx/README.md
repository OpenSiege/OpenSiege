# Siege FX Tutorial

## Getting Started

By: Aaron 'Jomdom' Ransley
Created: 1/4/2003

### Introduction

This tutorial will cover a small bit of the language used to create effects in Dungeon Siege.  Siege FX.  Fire from the torches on grimy dungeon walls, healing spells you cast frantically to stay alive, the glow on that new sword you just got from a chicken run, it's all done with Siege FX.  Since I have been working with Siege FX for quite awhile now, and I thought its about time I put together a tutorial for those of you interested in it.

Please take note that I have never written a tutorial before, so don't expect to be glued to your monitor, although I will do my best to be as thorough as possible.

_When I give examples of code,_ they will be in a different font, and in red. _When I show full scripts, they will be formatted for the web, so don’t expect your script to look exactly like mine._

#### Lingo

\- **Base Effect:**  _All Siege FX scripts are built off 22 different base effects. Please check the Resources section at the bottom of this tutorial for a link to a full listing of the base effects._  
\- **Parameter:**  _A parameter is how we tell any of the base effects how to act. They can be used to manipulate various aspects of a base effect; size, speed, color, density, duration, etc._

##### Dissecting the Fire Parameter

We will start the tutorial by learning how to use one of the most versatile of all the Siege FX base effects, fire. This base effect supports many different parameters that allow you to manipulate the way it acts.  Let’s find an effect that keeps it simple for the time being...  Ah yes, this looks like a good effect script:

```
[effect_script*]  
{
      name = fire_pit_fire_small;

      script = [[  

            sfx create fire #TARGET
                 "instant()offset(0,.25,0)scale(1)ts(.5)";
            sfx start #POP;

            ]];
}
```

Alright, let's break it down...

All Siege FX scripts will be structured like so:

```
[effect_script*]  
{
      name = <name of script>;

      script = [[

            ...Code...

            ]];
}
```

With that in mind, let's talk about the base effect created here. We prefix the command to create the base effect with sfx, as we do with almost every command in Siege FX. What we are doing is telling the sfx engine to create fire at a certain place - in this case, at our target.

Next we start the creating the actual parameters that effect the way the base effect acts. We start it with a double quote, followed by these parameters:

\- **instant():** _This parameter causes the_ fire _base effect to be instantly at full flame, and skip any gradual gaining of density and speed._\- **offset(x,x,x):** _This parameter offsets the base effect from the location of it's creation._ _offset(__0,.25,0)_ _causes the flame to be offset along the Y axis (up and down) just a tad._\- **scale(x):** _This is the overall scale of the base effect. It affects the size of the flames more than anything._\- **ts(x):** _This parameter stands for the **T**ime **S**cale of the base effect. It can be used on almost any base effect to speed it up or slow it down._

Then we end it with another double quote and a semi-colon.

Alright, now that we have that done, lets take a look at what it looks like in the game... This effect is part of the main game resources, so you won't need to create a new tank unless you have decided to modify it ahead of time.

**You will need DSMOD.exe for the next few steps, so please make sure you have downloaded the Dungeon Siege Toolkit. You can find a link to the Dungeon Siege Toolkit at the bottom of this tutorial in the Resources section.**

Now, go ahead and fire up DSMOD.exe and get yourself into a map. Bring up the console by pressing the ~ key to the left of the 1 key. Type in: sfx run fire\_pit\_fire\_small.

![](images/image002.jpg)

Ack, you’re on fire! Put yourself out by typing sfx stop scripts.

**Playing with Fire**

Now that we've seen the effect in action, let’s try to modify it a little.

First we will change the color of the flame, then we will add a special parameter to cause it to last longer before fading away, then we will change the size of the flame. Sounds like a lot? Not really, I'll step you though it bit by bit.

When we first created the fire, we added no parameter to manipulate the color, so it used it's default base effect color. Different base effects have different default color values, and the default color for fire is an orange color. We can overwrite this default color by adding a special parameter. This parameter is known as color0. Let’s see take a look at a definition:

\- **color0(x,x,x):** _This parameter is simply a RGB (Red, Green, Blue)  value. The numbers range from 0 to 1, and go out to two decimal places. You might use_ color0(1,.5,.5) _to get a bright red, or_ color0(1,.5,1) _to get a dark-ish purple._

Now that we have that out of the way, you should have a basic understanding of what the color0 parameter does. Let’s put them to work, shall we?

We will need to create a new .gas file for this step... Go ahead and open up Tank Viewer and load up Logic.dsres. Extract ..\\world\\global\\effects\\environmental.gas to a directory of your choice.

If you wish to use the Bits folder, then just extract it to there. Using the Bits folder allows for rapid reloading of scripts while in game; saves a lot of time. The Bits folder is located in ..\\my documents\\dungeon siege\\.

If you wish to use traditional Tank files, this directory will be your Tank's source, so make sure you have the directory tree correct. Open us environmental.gas that you just extracted and search for fire\_pit\_fire\_small, once you have located it, select all of it and cut it (ctrl + x). Select the rest of the document (ctrl + a) and press the delete key. Now, paste the fire\_pit\_fire\_small script back into the now empty document. Give it a new name, by changing the name = fire\_pit\_fire\_small to something else.  Make sure you use no capitals or spaces. Save and close the document. Give it a new name as well. You might have noticed that there are a few other commands in the script that aren't listed above... I stripped down the script so it didn't contain anything we didn't need for the lesson, so you might want to just copy the script from the tutorial.

Now you are probably ready to actually do something. Open up the document again, and find the last parameter, ts(.5) and type in color0(1,.5,.5) directly after it, but not outside the quotation mark. Your script should now look like this:

\[effect\_script\*\]  
{  
      name = fire\_pit\_fire\_small;  
  
      script = \[\[  
  
            sfx create fire #TARGET                 "instant()offset(0,.25,0)scale(1)ts(.5)color0(1,.5,.5)";  
            sfx start #POP;  
  
            \]\];  
}

Save the script, Tank it if you aren't using the Bits folder, and open up DSMOD.exe. If you already have it open, you won't be able to Tank the files, close it down. If it's open and your using Bits, then make sure your not currently flaming, if you are, bring up the console and type sfx stop scripts to stop the effect script from running, then type sfx reload to reload any sfx scripts that have been changed since the last time they were loaded. Once you are done reloading the scripts (you won't need to reload the scripts if you didn't have DSMOD.exe open when editing the script), run your script like you did last time (sfx run yourscriptname), except use your new script name instead of fire\_pit\_fire\_small. You should see a red flame...

Ack! Well, since it isn't actually hurting, you don't need to put it out this time.

![](images/image001.jpg)

Observe the flame... Notice how it fades out so fast? Bah! Let’s change that...

If you have using the Bits folder, Leave DSMOD.exe open this time, minimize it (alt + tab if you’re in full screen). If you are using Tanks, you need to close it. Insert another parameter into your script. Just like you did before, add it before the first parameter, but inside the double quotation mark. Add the parameter, alphafade(.5) parameter to the script. Save the script, go back into DSMOD, stop the scripts, reload the scripts, and play it again. The flame should last about twice as long... Let’s see a definition:

\- **alphafade(****x):** _Using this parameter changes the length of time before the particle is faded from existence, pretty straight forward. The lower the value, the longer it lasts. The default value is 1._

Everything should be going smoothly, so let’s change the size of the flame. Go ahead and edit the script and change the scale(1) parameter to a higher value... See what you get.

It seems that we are done with our tutorial for the time being. Continue to edit the fire base effect by adding more and more parameters. Please check the Resources section below for some useful information.

**Resources**

These are various resources I find _very_ useful.

·  [Dungeon Siege's Main Site](http://www.dungeonsiege.com/index.shtml) (Take a look around. Contains various mod resources, as well as the Dungeon Siege Toolkit)

·  [Game-Editing.net Dungeon Siege Forums](http://www.game-editing.net/forum/index.php?c=3) (If you plan of modifying Dungeon Siege, this is the place to be. Make sure to make use of the search function.)

·  [Siege FX Siege University Course](http://www.dungeonsiege.com/su_212.shtml) (Contains a list of the 22 base effects, a list of all possible parameters for those 22 base effects, and more. A great resource indeed.)

·  [Dungeon Siege Cosmetics](http://dsc.conflict-imminent.com/) (Home of this tutorial.)

·  [Jomdom's E-Mail](mailto:jomdom@hotmail.com?subject=Siege%20FX%20Tutorial) (Feel free to e-mail me with _any_ questions you have about this tutorial.)

**In the Future**

This tutorial was very simple, I’ll admit. Expect more and more advanced tutorials in the future. I will try to explain everything that’s going on in any Siege FX script that is included in any of my tutorials, so as long as you pay attention, and _experiment,_ you won't have to worry about falling behind.

**Credits**

·  Icemage - You have really helped me with the more advanced aspects of Siege FX. I thank you, Icemage.

·  Gas Powered Games - You make a killer effects engine! Great job, can't wait for Dungeon Siege 2's version of Siege FX.

## The Stack

By: Aaron 'Jomdom' Ransley  
Created: 1/6/2003

**Introduction**

In this tutorial, we will be discussing the Stack. As always, any code will be in red and in a different font. I will try to keep code from within a text-heavy paragraph to a minimum, and save it for its own paragraph.

**Lingo**

\- **Stack:** _The stack is the device that Siege FX uses to store effect data. I can’t think of any way to explain better than Icemage once did._

_“__You can think of this like a stack of plates - as we create effects, we put each effect onto the stack, the same way you would put plates that have been dried in a stack.” - Icemage_

**The Stack**

_As Icemage said, “...as we create effects, we put each effect onto the stack…”_

_Let me elaborate. Say we create an orbiter, one of the 22 base effects. It would automatically be entered into the stack (if it doesn’t have any errors that is). Here is an example:_

\[effect\_script\*\]

{

      name  \=orbs;

      script      \=\[\[

                  sfx create orbiter #TARGET

                        "color0(1,1,1)scale(.25)radius(0)";

                  set $orb1 #PEEK;

                  sfx start #POP;

                  sfx create orbiter #TARGET

                        "color0(1,1,1)scale(.25)radius(0)";

                  set $orb2 #PEEK;

                  sfx start #POP;

                  sfx create orbiter #TARGET

                        "color0(1,1,1)scale(.25)radius(0)";

                  set $orb3 #PEEK;

                  sfx start #POP;

                  sfx create orbiter #TARGET

                        "color0(1,1,1)scale(.25)radius(0)";

                  set $orb4 #PEEK;

                  sfx start #POP;

                  \]\];

}

“Eh?” you say? No kidding.

Alright, first off, understand that this effect doesn’t actually do anything, and if it did, it wouldn’t consist of anything more than creating 4 white orbiters at its target, directly on top of each other.

So, lets break it down.

You see that after I create the orbiter, define it’s location of creation, and it’s parameters, I go on to say:

set $orb1 #PEEK;

sfx start #POP;

Let’s break this down further… By writing:

set $orb1 #PEEK;

I tell the Siege FX engine to create a variable (something to contain the effect for quick referencing) out of whatever is on top of the stack. It effectively peeks at the top of the stack to check what’s there… Since we just created an orbiter, it is currently king of the stack. While we are talking about variables, I might as well mention that whenever you define a variable, it should be structured like so:

set <name of the variable with $ in prefixing it> <what you are creating the variable from>;

This means that if you want to create a variable to store a color, you can do this:

set $green .3,1,.3;

And it will store that color value inside the variable for later use in the script.

Getting back on track... Next we have this line:

sfx start #POP;

This is telling the Siege FX engine to ‘start’ what ever is on top of the stack, but ‘pop’ it off so it’s not on the stack anymore. If we then tried to use another command to look for the effect, you wouldn’t find it, because someone knocked it off the stack, and made a huge mess! Some people!

Here is an example of an invalid way of using the #POP and #PEEK commands.

set $orb1 #POP;sfx start #PEEK;

We create a variable by using the data from the top of the stack, currently the data from the last effect we created, but ‘pop’ it off at the end, because we are supposedly done using it… Wrong.

The next line tries to start whatever is currently in the stack, but there is nothing there! Jeepers! It must be because someone made a mistake and knocked the effect off the stack too soon.

**Slick Stack Slingin’**

Think your hot stuff? Try this on for size:

\[effect\_script\*\]

{

      name  \=technicolor\_orbs;

      script      \=\[\[

                  set $color4 .5,.5,.5;

                  set $color3 0,1,0;

                  set $color2 0,0,1;

                  set $color1 1,0,0;

                  sfx create orbiter #TARGET

                        "color0($color4)scale(2)radius(0)";

                  sfx create orbiter #TARGET

                        "color0($color3)scale(2)radius(0)";

                  sfx create orbiter #TARGET

                        "color0($color2)scale(2)radius(0)";

                  sfx create orbiter #TARGET

                        "color0($color1)scale(2)radius(0)";

                  set $orb3 #POP;

                  set $orb2 #POP;

                  sfx start $orb2;

                  sfx start $orb3;

                  \]\];

}

What color will the pile of orbs be when the script is complete? If you can figure that out, you’re done with the tutorial. If you can’t, just keep decoding it, because everything here has been covered so far.

See you next time!

**Resources**

These are various resources I find _very_ useful.

·  [Dungeon Siege's Main Site](http://www.dungeonsiege.com/index.shtml) (Take a look around. Contains various mod resources, as well as the Dungeon Siege Toolkit)

·  [Game-Editing.net Dungeon Siege Forums](http://www.game-editing.net/forum/index.php?c=3) (If you plan of modifying Dungeon Siege, this is the place to be. Make sure to make use of the search function.)

·  [Siege FX Siege University Course](http://www.dungeonsiege.com/su_212.shtml) (Contains a list of the 22 base effects, a list of all possible parameters for those 22 base effects, and more. A great resource indeed.)

·  [Dungeon Siege Cosmetics](http://dsc.conflict-imminent.com/) (Home of this tutorial.)

·  [Jomdom's E-Mail](mailto:jomdom@hotmail.com?subject=Siege%20FX%20Tutorial) (Feel free to e-mail me with _any_ questions you have about this tutorial.)

**In the Future**

Expect more and more advanced tutorials in the future. Enough said J

**Credits**

·  Icemage - You have really helped me with the more advanced aspects of Siege FX. I thank you, Icemage.

·  Gas Powered Games - You make a killer effects engine! Great job, can't wait for Dungeon Siege 2's version of Siege FX.


## Targets

By: Aaron 'Jomdom' Ransley  
Created: 1/15/2003

**Introduction**

In this installment, we will be discussing targets. How do you get your effects to go where you want them to? Shout at the code and mash the keyboard? That works, but very rarely. Instead, I will attempt to teach you the correct way to assign effects to targets. You will need to be familiar with the Stack, so if you haven’t read that tutorial yet, [go ahead](http://dsc.conflict-imminent.com/dlcounter/odtrack.php?url=http://dsc.conflict-imminent.com/tutorials/sfx/the_stack.zip) and do that now.

_Notes: You might notice the lack of the Lingo section in this tutorial. The formatting is also a bit smaller than the last few tutorials. I hope it isn’t too dramatic of a change._

**Taking a Peek**

Please note this script is very un-optimized, it is being used because it is a good example of how targets work.

Let’s take a look:

\[effect\_script\*\]

{

      name  \= halo;

      script      \=\[\[

                  sfx create orbiter #TARGET

                        "radius(2.5)phi(0)iphi(40)itheta(0)color0(1,1,0)

                        offset(0,.3,0)scale(.05)invisible()";

                  sfx attach\_point #PEEK bip01\_head target;

                  set $orb1 #PEEK;

                  sfx start #POP;

                  sfx create fire #TARGET

                        "flamesize(3)color0(1,.3,1)color1(0,0,0)

                        scale(1)velocity(0,0,0)alphafade(.3)

                        count(100)accel(0,0,0)";

                  sfx rat #PEEK;

                  sfx target #PEEK $orb1 target;

                  sfx start #POP;

                  \]\];

}

**Break it Down**

Let’s go to the most important part of the script first; where the effect goes:

sfx create orbiter #TARGET

#TARGET and #SOURCE macros can yield different results depending on how the script is called into existence, and what it is run on. In this case, #TARGET is the player.

The next important thing in the script is where on the player the effect goes:

sfx attach\_point #PEEK bip01\_head target;

This tells the Siege FX engine (sfx) to attach (attach\_point) the effect (#PEEK) to the players head (bip01\_head) using the effect’s target (target).

Then, we have attaching the fire to the orbiters:

sfx rat #PEEK;

sfx target #PEEK $orb1 target;

It’s confusing isn’t it? Why do we use the (target) command on one effect, and the (attach\_point) command on the other? You can think of it as a hierarchy.

First, we have the player (#TARGET). The player has a bunch of special bones to attach to. We use (attach\_point) to get a certain bone we need (bip01\_head) and hook the effect to it.

Secondly, we have the player again (#TARGET). We don’t want the player this time though. Let’s tell the Siege FX engine (sfx) to remove all targets (rat) from the effect (#PEEK).

Now that we have a clean slate, we can reassign targets. This time, we don’t want to go any further than just assigning a target, because an orbiter doesn’t have any special bones to attach to. We tell the Siege FX engine (sfx) to use the command (target) on the effect (#PEEK) assign a new target ($orb1) on the effect’s target (target). Since there are no special bones to attach to, we are done assigning targets.

In the end, we have an orbiter rotating around the player’s head, with fire attached to it that fades out slowly, so it looks as though we have a halo above the players head. Like I said earlier, it isn’t needed. You could easily just use a fire base effect using the radius parameter to achieve almost the exact same thing.

That wraps it up for now!

**Resources**

These are various resources I find _very_ useful.

*     [Dungeon Siege's Main Site](http://www.dungeonsiege.com/index.shtml) (Take a look around. Contains various mod resources, as well as the Dungeon Siege Toolkit)
*     [Game-Editing.net Dungeon Siege Forums](http://www.game-editing.net/forum/index.php?c=3) (If you plan of modifying Dungeon Siege, this is the place to be. Make sure to make use of the search function.)
*     [Siege FX Siege University Course](http://www.dungeonsiege.com/su_212.shtml) (Contains a list of the 22 base effects, a list of all possible parameters for those 22 base effects, and more. A great resource indeed.)
*     [Dungeon Siege Cosmetics](http://dsc.conflict-imminent.com/) (Home of this tutorial.)
*     [Jomdom's E-mail](mailto:jomdom@hotmail.com?subject=Siege%20FX%20Tutorial) (Feel free to e-mail me with _any_ questions you have about this tutorial.)

**In the Future**

Expect the next tutorial to cover some sneaky tricks with the Siege FX engine.

**Credits**

Gas Powered Games - You make a killer effects engine! Great job! I can’t wait for Dungeon Siege 2's revision of Siege FX.

     Siege FX Tutorial #1 <!-- /\* Font Definitions \*/ @font-face {font-family:Wingdings; panose-1:5 0 0 0 0 0 0 0 0 0; mso-font-charset:2; mso-generic-font-family:auto; mso-font-pitch:variable; mso-font-signature:0 268435456 0 0 -2147483648 0;} /\* Style Definitions \*/ p.MsoNormal, li.MsoNormal, div.MsoNormal {mso-style-parent:""; margin:0in; margin-bottom:.0001pt; mso-pagination:widow-orphan; font-size:12.0pt; font-family:"Times New Roman"; mso-fareast-font-family:"Times New Roman";} h1 {mso-style-next:Normal; margin-top:12.0pt; margin-right:0in; margin-bottom:3.0pt; margin-left:0in; mso-pagination:widow-orphan; page-break-after:avoid; mso-outline-level:1; font-size:16.0pt; font-family:Arial; mso-font-kerning:16.0pt;} p.MsoListBullet2, li.MsoListBullet2, div.MsoListBullet2 {mso-style-update:auto; margin-top:0in; margin-right:0in; margin-bottom:0in; margin-left:.5in; margin-bottom:.0001pt; text-indent:-.25in; mso-pagination:widow-orphan; mso-list:l0 level1 lfo2; tab-stops:list .5in; font-size:12.0pt; font-family:"Times New Roman"; mso-fareast-font-family:"Times New Roman";} p.MsoBodyText, li.MsoBodyText, div.MsoBodyText {margin-top:0in; margin-right:0in; margin-bottom:6.0pt; margin-left:0in; mso-pagination:widow-orphan; font-size:12.0pt; font-family:"Times New Roman"; mso-fareast-font-family:"Times New Roman";} p.MsoBodyTextIndent, li.MsoBodyTextIndent, div.MsoBodyTextIndent {margin-top:0in; margin-right:0in; margin-bottom:6.0pt; margin-left:.25in; mso-pagination:widow-orphan; font-size:12.0pt; font-family:"Times New Roman"; mso-fareast-font-family:"Times New Roman";} p.MsoListContinue2, li.MsoListContinue2, div.MsoListContinue2 {margin-top:0in; margin-right:0in; margin-bottom:6.0pt; margin-left:.5in; mso-pagination:widow-orphan; font-size:12.0pt; font-family:"Times New Roman"; mso-fareast-font-family:"Times New Roman";} a:link, span.MsoHyperlink {color:blue; text-decoration:underline; text-underline:single;} a:visited, span.MsoHyperlinkFollowed {color:purple; text-decoration:underline; text-underline:single;} p.StyleArialBefore5ptAfter5pt, li.StyleArialBefore5ptAfter5pt, div.StyleArialBefore5ptAfter5pt {mso-style-name:"Style Arial Before\\: 5 pt After\\: 5 pt"; margin-top:5.0pt; margin-right:0in; margin-bottom:5.0pt; margin-left:0in; mso-pagination:widow-orphan; font-size:10.0pt; font-family:Arial; mso-fareast-font-family:"Times New Roman"; mso-bidi-font-family:"Times New Roman";} span.SpellE {mso-style-name:""; mso-spl-e:yes;} span.GramE {mso-style-name:""; mso-gram-e:yes;} /\* Page Definitions \*/ @page {mso-page-border-surround-header:no; mso-page-border-surround-footer:no;} @page Section1 {size:8.5in 11.0in; margin:1.0in 1.25in 1.0in 1.25in; mso-header-margin:.5in; mso-footer-margin:.5in; mso-paper-source:0;} div.Section1 {page:Section1;} /\* List Definitions \*/ @list l0 {mso-list-id:-125; mso-list-type:simple; mso-list-template-ids:205841210;} @list l0:level1 {mso-level-number-format:bullet; mso-level-style-link:"List Bullet 2"; mso-level-text:\\F0B7; mso-level-tab-stop:.5in; mso-level-number-position:left; text-indent:-.25in; font-family:Symbol;} @list l1 {mso-list-id:-2; mso-list-type:simple; mso-list-template-ids:572177678;} @list l1:level1 {mso-level-start-at:0; mso-level-text:\*; mso-level-tab-stop:none; mso-level-number-position:left; margin-left:0in; text-indent:0in;} @list l2 {mso-list-id:616988282; mso-list-type:hybrid; mso-list-template-ids:-487549960 67698689 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l2:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol;} @list l3 {mso-list-id:666788475; mso-list-type:hybrid; mso-list-template-ids:-1698530994 1868184898 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l3:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B6; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol;} @list l4 {mso-list-id:872115448; mso-list-template-ids:-487549960;} @list l4:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol;} @list l4:level2 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:1.5in; mso-level-number-position:left; margin-left:1.5in; text-indent:-.25in; font-family:"Courier New";} @list l4:level3 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:2.0in; mso-level-number-position:left; margin-left:2.0in; text-indent:-.25in; font-family:Wingdings;} @list l4:level4 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:2.5in; mso-level-number-position:left; margin-left:2.5in; text-indent:-.25in; font-family:Symbol;} @list l4:level5 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:3.0in; mso-level-number-position:left; margin-left:3.0in; text-indent:-.25in; font-family:"Courier New";} @list l4:level6 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:3.5in; mso-level-number-position:left; margin-left:3.5in; text-indent:-.25in; font-family:Wingdings;} @list l4:level7 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:4.0in; mso-level-number-position:left; margin-left:4.0in; text-indent:-.25in; font-family:Symbol;} @list l4:level8 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:4.5in; mso-level-number-position:left; margin-left:4.5in; text-indent:-.25in; font-family:"Courier New";} @list l4:level9 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:5.0in; mso-level-number-position:left; margin-left:5.0in; text-indent:-.25in; font-family:Wingdings;} @list l5 {mso-list-id:1001858529; mso-list-type:hybrid; mso-list-template-ids:-259894358 350631756 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l5:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:22.5pt; mso-level-number-position:left; margin-left:22.5pt; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l6 {mso-list-id:1039234215; mso-list-type:hybrid; mso-list-template-ids:-446526342 67698689 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l6:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:.5in; mso-level-number-position:left; text-indent:-.25in; font-family:Symbol;} @list l7 {mso-list-id:1166088963; mso-list-template-ids:705214696;} @list l7:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l7:level2 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:1.5in; mso-level-number-position:left; margin-left:1.5in; text-indent:-.25in; font-family:"Courier New";} @list l7:level3 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:2.0in; mso-level-number-position:left; margin-left:2.0in; text-indent:-.25in; font-family:Wingdings;} @list l7:level4 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:2.5in; mso-level-number-position:left; margin-left:2.5in; text-indent:-.25in; font-family:Symbol;} @list l7:level5 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:3.0in; mso-level-number-position:left; margin-left:3.0in; text-indent:-.25in; font-family:"Courier New";} @list l7:level6 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:3.5in; mso-level-number-position:left; margin-left:3.5in; text-indent:-.25in; font-family:Wingdings;} @list l7:level7 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:4.0in; mso-level-number-position:left; margin-left:4.0in; text-indent:-.25in; font-family:Symbol;} @list l7:level8 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:4.5in; mso-level-number-position:left; margin-left:4.5in; text-indent:-.25in; font-family:"Courier New";} @list l7:level9 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:5.0in; mso-level-number-position:left; margin-left:5.0in; text-indent:-.25in; font-family:Wingdings;} @list l8 {mso-list-id:1279751207; mso-list-template-ids:-1698530994;} @list l8:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B6; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol;} @list l8:level2 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:1.5in; mso-level-number-position:left; margin-left:1.5in; text-indent:-.25in; font-family:"Courier New";} @list l8:level3 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:2.0in; mso-level-number-position:left; margin-left:2.0in; text-indent:-.25in; font-family:Wingdings;} @list l8:level4 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:2.5in; mso-level-number-position:left; margin-left:2.5in; text-indent:-.25in; font-family:Symbol;} @list l8:level5 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:3.0in; mso-level-number-position:left; margin-left:3.0in; text-indent:-.25in; font-family:"Courier New";} @list l8:level6 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:3.5in; mso-level-number-position:left; margin-left:3.5in; text-indent:-.25in; font-family:Wingdings;} @list l8:level7 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:4.0in; mso-level-number-position:left; margin-left:4.0in; text-indent:-.25in; font-family:Symbol;} @list l8:level8 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:4.5in; mso-level-number-position:left; margin-left:4.5in; text-indent:-.25in; font-family:"Courier New";} @list l8:level9 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:5.0in; mso-level-number-position:left; margin-left:5.0in; text-indent:-.25in; font-family:Wingdings;} @list l9 {mso-list-id:1592081694; mso-list-type:hybrid; mso-list-template-ids:-1191432996 1105770856 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l9:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:58.5pt; mso-level-number-position:left; margin-left:58.5pt; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l10 {mso-list-id:1615356967; mso-list-template-ids:-1698530994;} @list l10:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B6; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol;} @list l10:level2 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:1.5in; mso-level-number-position:left; margin-left:1.5in; text-indent:-.25in; font-family:"Courier New";} @list l10:level3 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:2.0in; mso-level-number-position:left; margin-left:2.0in; text-indent:-.25in; font-family:Wingdings;} @list l10:level4 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:2.5in; mso-level-number-position:left; margin-left:2.5in; text-indent:-.25in; font-family:Symbol;} @list l10:level5 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:3.0in; mso-level-number-position:left; margin-left:3.0in; text-indent:-.25in; font-family:"Courier New";} @list l10:level6 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:3.5in; mso-level-number-position:left; margin-left:3.5in; text-indent:-.25in; font-family:Wingdings;} @list l10:level7 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:4.0in; mso-level-number-position:left; margin-left:4.0in; text-indent:-.25in; font-family:Symbol;} @list l10:level8 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:4.5in; mso-level-number-position:left; margin-left:4.5in; text-indent:-.25in; font-family:"Courier New";} @list l10:level9 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:5.0in; mso-level-number-position:left; margin-left:5.0in; text-indent:-.25in; font-family:Wingdings;} @list l11 {mso-list-id:1689990809; mso-list-type:hybrid; mso-list-template-ids:1654426930 1105770856 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l11:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:22.5pt; mso-level-number-position:left; margin-left:22.5pt; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l12 {mso-list-id:1835873538; mso-list-type:hybrid; mso-list-template-ids:-1977431558 -446527790 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l12:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Symbol; color:windowtext;} @list l13 {mso-list-id:1915629704; mso-list-type:hybrid; mso-list-template-ids:705214696 1105770856 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l13:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:1.0in; mso-level-number-position:left; margin-left:1.0in; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l14 {mso-list-id:2018462080; mso-list-template-ids:-1191432996;} @list l14:level1 {mso-level-start-at:0; mso-level-number-format:bullet; mso-level-text:-; mso-level-tab-stop:58.5pt; mso-level-number-position:left; margin-left:58.5pt; text-indent:-.25in; font-family:Arial; mso-fareast-font-family:"Times New Roman";} @list l14:level2 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:1.5in; mso-level-number-position:left; margin-left:1.5in; text-indent:-.25in; font-family:"Courier New";} @list l14:level3 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:2.0in; mso-level-number-position:left; margin-left:2.0in; text-indent:-.25in; font-family:Wingdings;} @list l14:level4 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:2.5in; mso-level-number-position:left; margin-left:2.5in; text-indent:-.25in; font-family:Symbol;} @list l14:level5 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:3.0in; mso-level-number-position:left; margin-left:3.0in; text-indent:-.25in; font-family:"Courier New";} @list l14:level6 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:3.5in; mso-level-number-position:left; margin-left:3.5in; text-indent:-.25in; font-family:Wingdings;} @list l14:level7 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:4.0in; mso-level-number-position:left; margin-left:4.0in; text-indent:-.25in; font-family:Symbol;} @list l14:level8 {mso-level-number-format:bullet; mso-level-text:o; mso-level-tab-stop:4.5in; mso-level-number-position:left; margin-left:4.5in; text-indent:-.25in; font-family:"Courier New";} @list l14:level9 {mso-level-number-format:bullet; mso-level-text:\\F0A7; mso-level-tab-stop:5.0in; mso-level-number-position:left; margin-left:5.0in; text-indent:-.25in; font-family:Wingdings;} @list l15 {mso-list-id:2018799497; mso-list-type:hybrid; mso-list-template-ids:1440508192 67698689 67698691 67698693 67698689 67698691 67698693 67698689 67698691 67698693;} @list l15:level1 {mso-level-number-format:bullet; mso-level-text:\\F0B7; mso-level-tab-stop:58.5pt; mso-level-number-position:left; margin-left:58.5pt; text-indent:-.25in; font-family:Symbol;} @list l1:level1 lfo1 {mso-level-number-format:bullet; mso-level-numbering:continue; mso-level-text:\\F0B7; mso-level-tab-stop:none; mso-level-number-position:left; mso-level-legacy:yes; mso-level-legacy-indent:0in; mso-level-legacy-space:0in; margin-left:0in; text-indent:0in; font-family:Symbol;} ol {margin-bottom:0in;} ul {margin-bottom:0in;} -->

## Script Architecture/Terminology

By: Aaron 'Jomdom' Ransley, and Trailstorm Radeem.  
Created: 1/10/003

**Introduction**

This document will cover script structure and terminology, invented by myself and Trailstorm Radeem. We feel that the Dungeon Siege community has no real terminology to use in conversations relating to Siege FX scripts. We will list terminology and structure for an entire Siege FX script, start to finish. Proper indentation practices are also shown as the script progresses. Also, keep in mind that this is meant to be a reference more than a tutorial, so not every aspect of Siege mentioned will be explained with great detail.

_Note: Some Siege FX commands already have names, or don’t need any. We will be covering mostly uncharted territory, although you might see a few little tidbits that we decided to rename. We hope this doesn’t cause any confusion._

We really hope this reference will become a standard, so if you enjoy it, _please_ do your best to pass it on to anyone that is interested in Siege FX. If you need to link to the .zip file on my site, please use the following URL:

http://dsc.conflict-imminent.com/dlcounter/odtrack.php?url=http://dsc.conflict-imminent.com/tutorials/sfx/term\_reference.zip

**\-  Script Architecture -**

Here, we are going to go over the names of the different sections of a Siege FX script.

**Script Header**

The script header comprises of this section on code:

\[efect\_script\*\]

{

name = example\_script; //Define the name of the script.

script      \=\[\[ //Open the script.

**Declaration**

In the declaration section, we create variables for use in the script:

set $color0 1,1,.3; //Store the value 1,1,.3 inside the variable, $color0 for use in the script later on.set $size \[0\]; //Get the first p-param in the parameter string, whatever that may be.

**Effect Block: Pre-Block**

In the pre-block section, we will declare the base effect, and it’s location like so:

create fire #TARGET //Create a fire base effect at #TARGET.

**Effect Block: Parameters**

In the parameter block, we define all the parameters for base effect it is paired with:

“instant()color0($color0)scale($size)ts(3)”; //Use these parameters in the parameter block.

**Effect Block: Post-Block**

In the post-block, we set out targets, store our completed effect in a variable, and anything else that is required before you start the effect:

sfx rat #PEEK; //Remove all targets from the fire’s memory.sfx target #PEEK ap\_trace01 target; //Redefine the target, creating the fire to attach to the end of the weapon the script is called on.  
set $fire #PEEK; //Store the effect in a variable. Not actually needed, just explaining how to do it.sfx start #POP; //Start the effect.

**Script Footer**

In the script footer, we wrap up the script:

\]\]; //Close the script.

}

Take note of the block pre and post notations. All the data from the pre-block to the end of the post-block is considered an effect block. This script comprises of 1 effect block.

 **\- Further Terminology -**

Now that we have covered the architecture of a Siege FX script in detail, let’s break it down further with terminology.

**Passed Variables**

Variables passed between/into scripts using this method are called pass-parameters, or p-params. Using this method won’t be explained here, but is a _very_ good way to optimize your scripts. Think of creating a script that looks for a p-param for it’s color, speed, scale, ect. Think of being able to call the script from a template, putting data into those p-params to be passed. You could use the script over and over again and get different looks each time, because your p-params are different. Trailstorm Radeem, a close friend of mine, and the co-author of this reference, is a true pioneer of utilizing this method.

Here is syntax example of grabbing a p-param after its data has been defined in the template:

set $scale \[0\]; //Store the first value that was passed into the script from the template in the variable named $scale.

**Repeater Scripts**

A repeater script is something that I have used, and plan to continue using them, in my Cosmetics line of mods. There really isn’t an analogy I can use to explain it, so I have to get technical. A repeater script works like so:

·        The repeater script is initially started.

·        The script then calls what is to be repeated.

·        Then the repeater script calls itself over again and repeats the process.

Lets built on this, and take a look at a script that uses this method:

//MAGNETO STAFF\\\\

\[effect\_script\*\]

{

      name  \=abr\_mag\_staff;

      script      \=\[\[

                  if(!#WEAPON\_TYPE\_STAFF)  
                  {

exit;              
}

                  pause .5; //Pause the script for half a second.

                  call mag\_curve; //Call the script that is to be repeated.

                  call abr\_mag\_staff; //Call this same script over again.

                  \]\];

}

//MAGNETO STAFF CURVE\\\\

\[effect\_script\*\]

{

      name  \=mag\_curve;

      script      \=\[\[

                  sfx create curve #TARGET

                        "curvature(1)model(0)scale(.1)color0(1,.5,.8)

                        ts(2)tlength(20)";

                  sfx target #PEEK target;

                  sfx attach\_point #PEEK ap\_trace01 target;

                  sfx attach\_point #PEEK ap\_trace02 source;

                  sfx start #POP;

                  \]\];

}

There isn’t much explaining to do… The script ends up repeating over and over again with a half second delay. The reason we don’t just call ‘mag\_curve’ over and over again from itself is simply because the repeater script acts as a great hub if you wish to control many repeating scripts from there. There are times that you do not need a repeater script, though.

**Resources**

These are various resources I find _very_ useful.

·  [Dungeon Siege's Main Site](http://www.dungeonsiege.com/index.shtml) (Take a look around. Contains various mod resources, as well as the Dungeon Siege Toolkit)

·  [Game-Editing.net Dungeon Siege Forums](http://www.game-editing.net/forum/index.php?c=3) (If you plan of modifying Dungeon Siege, this is the place to be. Make sure to make use of the search function.)

·  [Siege FX Siege University Course](http://www.dungeonsiege.com/su_212.shtml) (Contains a list of the 22 base effects, a list of all possible parameters for those 22 base effects, and more. A great resource indeed.)

·  [Dungeon Siege Cosmetics](http://dsc.conflict-imminent.com/) (Home of this tutorial.)

·  [Jomdom's E-mail](mailto:jomdom@hotmail.com?subject=Siege%20FX%20Tutorial) (Feel free to e-mail me with _any_ questions you have about this tutorial.)

**In the Future**

Although this reference doesn’t really fit into the progression of the tutorials I am doing, it isn’t meant to. More tutorials are on their way, so hold tight.

**Credits**

·  Icemage - Thanks for the input on the terminology.

·  Trailstorm Radeem - It was a great idea to make such a reference!

·  Gas Powered Games - You make a killer effects engine! Great job! Can’t wait for Dungeon Siege 2's version of Siege FX.