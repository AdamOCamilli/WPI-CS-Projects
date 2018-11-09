# Final Proposal - Incentive Planner
## Idea
	This is what I've tried to build up in a2 and a3 to varying success. The idea is an interactive planning and calendar application that allows the preemptive planner to preprogram rewards or incentives for themselves to complete tasks. Essentially, the app is an interactive animated planner associated with a user account that allows users to schedule events and reminders as in any other calendar app but has rewards for checking tasks off that can be set by yourself or by your "friends" within the app. These rewards are the fun part: I want them to be able to be almost anything. A simple congratulatory, a funny video, a cool animation, an embedded game, a geolocation of something cool, a Venmo request (i.e. a kid does his homework, mom's reward is a venmo request with special text so she knows it's for the task), whatever I can put in in the time I have for the final.
	
## Planned Features
	- Full stack web app with dynamic pages
	- User accounts and authentification.
	- Geolocation (with free google maps api key)
	- Simple animations for planner (page turning, checking off)
	- Many plugins (youtube embeds, steam and amazon links if I can get them, definitely some simple games)

## Plan
	- I will build outline with `express`.
	- I plan to use `bcrypt` and `passport` for user authentification (probably will use google or facebook passport strategy).
	- I plan to obtain a Google API key and make not to exceed free map loads (25,000/day)
