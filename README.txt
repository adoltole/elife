DESCRIPTION

Elife is a drush script designed to automate certain repetitive drupal tasks.

=========================================

INSTRUCTIONS

1- Clone this repo in your /home/user/.drush/ folder and run "drush cache-clear drush"
2- Create a file called "vars.php" using "default.vars.php" included in the repository (cp default.vars.php vars.php) and modify the content with your data.
3- Copy the default vitualhost "defaultapache-pre.conf" or "defaultapache-pro.conf" depending on the kind of server (production or preproduction) to /etc/apache2/sites-available/. The "defaultapache-pre.conf" template is designed for a preproduction site where authentication is required.


