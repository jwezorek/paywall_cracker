Many pay-walled news articles on the web can be read by hitting ESC at just the right time when the article is loading.

This is code for a simple Win32 application that has controls for spamming ESC programmatically. Specifically, it has controls for setting two integer times in milliseconds A and B and a count C such that when I push a button on it it makes Chrome the foreground window, issues a synthetic keystroke to cause Chrome to reload the current page, then waits A milliseconds and begins issuing synthetic ESC key presses every B milliseconds for C times.
