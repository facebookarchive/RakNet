<?php
//These vars may be changed to fit your needs.
//User executing the script must have write access to the file directory.
define("PASSWORD_FILE", "pw");
define("RECORD_FILE", "RecordFile.data.php");
define("TIMEOUT", 60);

include("lib/PasswordManager.class.php");
include("lib/RecordManager.class.php");

$pm = new PasswordManager();

//Check if password exists. If it doesn't exist create the password form for the user
//to input their passwords.
if(!file_exists(PASSWORD_FILE)){		
	$error = "";
	$message = "";
	
	if(!empty($_POST)){
		$error = $pm->validatePasswords($_POST);
		if(empty($error)){
			$pm->savePasswords($_POST);
			header("Location: DirectoryServer.php");
			exit;
		}
	}
	
	echo $pm->generateCss();
	echo $error;
	echo $pm->generatePasswordForm();
}
else{
	//If the password file exists use controller logic
	$rm = new RecordManager();
	$passwords = $pm->getPasswords();	
	$rm->expireRecords();
	
	//If upload mode and uploadPassword matches	
	if(!empty($_GET["query"]) && $_GET["query"] == "upload"){		
		if(isset($_GET["uploadPassword"]) && md5($_GET["uploadPassword"]) == $passwords["uploadPassword"]){
			//Use the following code to read post body. Not regular form posts which would be accessed with $_POST						
			$postText = trim(file_get_contents('php://input'));	
								
			$output = $rm->uploadRecords($postText); 						
			echo $output;			
		}
	}
	//If download mode and downloadPassword matches
	elseif(!empty($_GET["query"]) && $_GET["query"] == "download"){		
		if(isset($_GET["downloadPassword"]) && md5($_GET["downloadPassword"]) == $passwords["downloadPassword"]){						
			 
			$output =  $rm->downloadRecords();			
			echo $output;
		}
	}
	//If upDown mode and downloadPassword and uploadPassword matches
	elseif(!empty($_GET["query"]) && $_GET["query"] == "upDown"){
		if(isset($_GET["downloadPassword"]) && md5($_GET["downloadPassword"]) == $passwords["downloadPassword"] 
			&& isset($_GET["uploadPassword"]) && md5($_GET["uploadPassword"]) == $passwords["uploadPassword"] ){
//				Use the following code to read post body. Not regular form posts which would be accessed with $_POST
				$postText = trim(file_get_contents('php://input') );							
				
				$output =  $rm->downloadRecords();				
				echo $output;
				
				$output =  $rm->uploadRecords($postText);				
				echo $output;
		}
	}
	//Else you're in view mode just display the records with html and css
	else{
		echo $rm->generateCss();
		echo $rm->viewRecords();
	}	
}


?>
