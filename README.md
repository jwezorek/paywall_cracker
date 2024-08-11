Many pay-walled news articles on the web can be read by hitting ESC at just the right time when the article is loading.

This is code for a simple Win32 application that has controls for spamming ESC programmatically. Specifically, it has controls for setting two integer times in milliseconds A and B such that when you push the "crack it" button, the program
* makes Chrome the foreground window
* issues a synthetic keystroke to cause Chrome to reload the current page
* waits A milliseconds
* begins issuing synthetic ESC key presses every B milliseconds until canceled by the user, unless it is in "pulse" mode in which it issues a short burst of ESC keypresses and does not need to be canceled.
