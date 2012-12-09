F.aceBash
=========

A command line Facebook client for the tech oriented socialite.

The initial design of this program was to keep it as light weight as possible, however some packages are required to build F.aceBash. I will try to list the sub dependencies of each package, however they may not be accurate.

* Python 2.7 (for the login script)
* libboost
* libcurl
* cURLpp
* Mechanize
  * setuptools for easy_install (Python package to install eggs)
* JsonCpp
  * scons (for compiling)
* GNU Readline
  * Ncurses

Installation
============

To install dependencies run `install_deps.sh` as super user. If that fails you may have to install the dependencies manually.

To build and install run `./configure`, `make`, and `make install`.

Features
========

F.aceBash implements many of the common interactions with Facebook. A user can log in and out, their news feed and any of their frends' news feeds. It allows posting statuses on any news feed and commenting on any story that Facebook allows. Liking any story or comment is also supported. In addition F.aceBash can be used as a batch image and video uploader, allowing the user to upload any photo that Facebook supports to any album with any discription text. F.aceBash also supports batch actions allowing a user to string together commands into one run of the program. For example, use `-ln` to log in and display your news feed in one command. Actions can also have a number of iterations assigned to them. `-n100` displays the logged in user's news feed 100 times consecutively, polling for changes each time. `-lnskc` logs in a user, displays their news feed, posts a status, likes that status, and posts a comment on that status in one command.

Commands
========

    -c, --comment   Allows the user to comment on a post
                    --val message Specifies the text to post as the comment. This enables "quite mode" where the status being commented on is not shown and the user is not prompted.
                    --num number  An integer that represents the index of the post to comment on.
                    --who person  The name of the friend whose wall the post to be commented on is on. Use "me" if the post to be commented on is on your own wall.
    -h, --help      Shows the version number and help text.
    -l, --login     Prompts the user to enter credentials to log into Facebook.
                    --user email  The email address (user name) to log into Facebook with.
                    --pass pass   The password to log in with. It is recommended not to use this flag as the password will be visible in history as well as 'w' and 'who' (among other) commands, but it can be useful for scripting. Don't say you weren't warned.
        --logout    Logs the user out of Facebook.
    -k, --like      Likes a post or comment.
                    --num number  A number in the form [post index].[comment index] . If no post index is given 1, or the most recent post, is assumed. If no comment index is given (even if a period is provided) only the post index is used.
                    --who person  The name of the friend whose wall the post or comment to be liked is on. Use "me" if the post or comment to be liked is on your own wall.
                    --force_yes   If this argument is passed "quite mode" is enabled and the user is not asked for confirmation on the item to be liked.
    -n, --show_news_feed
                    Shows as many items of a news feed as can be found, including comments and likes.
                    --who person  The name of the friend whose wall is to be shown. Use "me" to show your own wall.
                    --num range   Range is a set of numbers. Sets of numbers are comma delimited. A set can be one number, a fixed span, or an arbitrary span. A fixed span is two '-' seperated numbers where each number is the range is added to the list inclusivly. An arbitrary span is one number with a '*' either before or after it. If the '*' is before the number, then all numbers between 0 and the following number inclusively are added to the list. If the '*' is after the number, all items available after and including that number are added to the list.
    -s, --update_status
                    Allows a user to post a status to their wall.
                    --who person  The name of the friend whose wall the status should be posted on. "me" is not excepted for this command.
                    --val message The content of the status to post. If this argument is given "quite mode" is enabled and the user is not prompted for a status message.
    -u, --upload    Allows a user to upload a set of images to an album and/or a set of videos to their account.
                    --album name  The name of the album to upload photos to. If none provided "F.aceBash" is used by default. If the specified album does not exist it is created. Videos do not get uploaded to albums so if only videos are being uploaded this argument is not needed and will just be ignored if provided.
                    --val message The description to apply to the album. If a new album is created this message is passed as the description of the new album. If the album alread exists this value is ignored and the user is notified.
                    --list        If this flag is set all other arguments are ignored and the only action taken is to list all available albums.
                    --img files   This specifies the list of files to upload to whichever album has been specified. This list can contain any number of image files. No code is implemented client-side to check valid file types.
                    --vid files   This specifies the list of files to upload to Facebook as videos. This list can contain any number of video files. No code is implemented client-side to check valid file types.
    -v, --version   Displays only version and copyright information.