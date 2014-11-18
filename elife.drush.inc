<?php
/**
 * Implementation of hook_drush_command()
 */

function elife_drush_command() {
  $items['elife'] = array(
    'description' => 'Shows all elife commands',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  $items['elifeupdate'] = array(
    'description' => 'updates ELIFE to the last version',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['mount-pre'] = array(
    'description' => 'mount a new preproduction enviroment and apache virtualhost',
    'arguments' => array (
      'URL' => 'The URL indicator',
    ),
    'requiered-arguments' => FALSE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['mount-pro'] = array(
    'description' => 'mount a new production enviroment and apache virtualhost',
    'arguments' => array (
      'URL' => 'The URL indicator',
    ),
    'requiered-arguments' => FALSE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, 
   );
 $items['mount'] = array(
    'description' => 'mount a new production enviroment and apache virtualhost',
    'arguments' => array (
      'URL' => 'The URL indicator',
      'env' => 'The enviroment indicator',
    ),
     'requiered-arguments' => FALSE,
     'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,                   
   );
 
return $items;
}

/**
 * Elife command callback.
 */
function drush_elife($number = NULL) {
  drush_print ('elife       -> shows this help',2);
  drush_print ('mount-pre -> create a new preproduction enviroment and apache virtualhost',2);
  drush_print ('mount-pro -> create a new production enviroment and apache virtualhost',2);
  drush_print ('elifeupdate -> updates ELIFE to the last version',2);
}

/**
 * elifeupdate command callback.
 */
function drush_elife_elifeupdate() {
  drush_shell_exec("cd ~/.drush/elife && git pull");
  drush_shell_exec("drush cc drush");
  foreach (drush_shell_exec_output() as $line) {
    $out .=$line."\n\r";
  }
  return $out;
}

/**
 * mount command callback.
 */
function drush_elife_mount($url=NULL,$env=NULL) {
include 'vars.php';
  $pieces=explode('/',$url);
  $size=sizeof($pieces);
  $last=$pieces[$size -1 ];
  $pieces_last=explode('.',$last);
  $project_name = $pieces_last[0];

  $project_name = drush_prompt("What's the project name", $project_name);
  drush_print ('I am cloning the repository into your machine, it can take a bit of time');
  drush_shell_exec_interactive('sudo -u '.$deployer_user.' git clone '.$url.' '.$project_name);

  $random = substr(str_shuffle("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0, 7);

  drush_print ('Now enter some parameters for the development site, please\n\r');
  $admin_pass = $random;
  $site_name = ('unnamed');
  $site_email = 'my@example.com';
  $database_user = substr ($project_name, 0, 16);
  $database_user_pw = $random;

//Site install pre and pro
  drush_shell_exec_interactive("cd ".$project_name."/httpdocs && sudo -u '.$deployer_user.' drush site-install minimal --site-name=$site_name --account-pass=$admin_pass --db-su=$database_root --db-su-pw=$database_root_pw --db-url=mysql://$database_user:$database_user_pw@localhost/".$env."_$project_name");

//Create new site in Apache pre and pro
  if ($env==pre){
  drush_shell_exec_interactive("sudo cp /etc/apache2/sites-available/defaultapache-$env.conf /etc/apache2/sites-available/$project_name.$DomainTermination.conf");
  drush_shell_exec_interactive("sudo sed -i 's/PROJECT/$project_name/g' /etc/apache2/sites-available/$project_name.$DomainTermination.conf");
  drush_shell_exec_interactive("sudo sed -i 's/AUTHNAME/$AuthName/g' /etc/apache2/sites-available/$project_name.$DomainTermination.conf");
  drush_shell_exec_interactive("sudo sed -i 's|AUTHUSERFILE|$AuthUserFile|g' /etc/apache2/sites-available/$project_name.$DomainTermination.conf");
  drush_shell_exec_interactive("sudo sed -i 's/DOMAINTERMINATION/$DomainTermination/g' /etc/apache2/sites-available/$project_name.$DomainTermination.conf");
  }
else {
  $newdomain = drush_prompt("What's the new web domain without TLD (example: myweb, www.newweb, ...)", $project_name);
  $TLD = drush_prompt("What's the TLD without '.' (com by default)",'com');
  drush_shell_exec_interactive("sudo cp /etc/apache2/sites-available/defaultapache-$env.conf /etc/apache2/sites-available/$newdomain.$TLD.conf");
  drush_shell_exec_interactive("sudo sed -i 's/PROJECT/$project_name/g' /etc/apache2/sites-available/$newdomain.$TLD.conf");
  drush_shell_exec_interactive("sudo sed -i 's/DOMAIN/$newdomain/g' /etc/apache2/sites-available/$newdomain.$TLD.conf");
  drush_shell_exec_interactive("sudo sed -i 's/TLD/$TLD/g' /etc/apache2/sites-available/$newdomain.$TLD.conf");
  drush_print ('The new environment has been created, you can change your virtualhost manually if needed');
 }

//Enable the new site  
  if ($env==pre){  
  drush_shell_exec_interactive("sudo a2ensite $project_name.$DomainTermination.conf && sudo service apache2 reload");
  }
  else {
  drush_shell_exec_interactive("sudo a2ensite $newdomain.$TLD.conf && sudo service apache2 reload");
  }

//Set correct user of drupal root
  drush_shell_exec_interactive("sudo chown $deployer_user:www-data /var/www/$project_name -R && sudo chown www-data:www-data /var/www/$project_name/httpdocs/sites/default/files -R");

}
/**
* mount-pre command callback.                                                                                                                                
*/

function drush_elife_mount_pre($url=NULL, $env='pre') {
  if ($url==NULL){
    $git_repository = drush_prompt("What's the URL of remote git repository");
  }
  else{
     $git_repository= $url;
  }
  drush_shell_exec_interactive("drush mount $git_repository $env");
}

/**
* mount-pro command callback.
*/

function drush_elife_mount_pro($url=NULL, $env='pro') {
  if ($url==NULL){
   $git_repository = drush_prompt("What's the URL of remote git repository");
  }
  else{
   $git_repository= $url;
  }
  drush_shell_exec_interactive("drush mount $git_repository $env");                                                                                         
}
