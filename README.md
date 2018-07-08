It was written when i was a freshman of my university. Now 7 years flies.

Although high-precision algorithm of integer can be seen everywhere, it's hard to see an algorithm not simply based on 10. I wrote it based on 10^4 to have better performance. And in division, direct division with postprocess is used in stead of triad dvision or subtraction, which are common method to achieve division with high-precision.

And a text shell with porland expression is used to resolve the input.

No Makefile is used due ot too simple code. You can use any compile method.

Eg.
	gcc main.cpp num.cpp
