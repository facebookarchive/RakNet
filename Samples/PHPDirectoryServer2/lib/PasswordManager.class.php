<?php
class PasswordManager{
	
	function PasswordManager(){
		
	}
	
	function generateCss(){
		 $output = "<html>
		 			<head>
		 			<style type='text/css' media='all'>
		 				#password-form{
		 					border:1px solid #1E3C64;
		 					background:#4A8AD1;
		 					padding:5px 10px;
		 					color:#EFF7FF;
		 				}
		 				#password-form .text-field{
		 					border:1px solid #bbb;
		 					padding:3px;
		 				}
		 				.required{
		 					color:red;
		 				}
		 				.message{
		 					display:block;
		 					margin:5px 0;
		 					padding:5px;		 					
		 				}
		 				.error{
		 					background:#FFEFF0;		 					
		 					color:red;
		 				}
		 				#password-form .submit-button{
		 					background:#F6F5F5;
		 					color:#666;		 					
		 					padding:5px;
		 					border: 1px solid #bbb;
		 				}
		 			</style>
		 			</head>
		 			<body>";		 
		 return $output;
	}
	
	function generatePasswordForm(){									
		$output = "<h2>Admin Password Setup</h2><form id='password-form' action='' method='POST' >
						<p><label>Upload Password<span class='required'>*</span> </label><input class='text-field' type='text' name='uploadPassword' /></p>
						<p><label>Download Password<span class='required'>*</span> </label><input class='text-field' type='text' name='downloadPassword' /></p>
						<p><input type='submit' value='Submit' class='submit-button' /></p>					 
					</form>
					</body>
					</html>";
		
		return $output;
	}
	
	function validatePasswords($post_array){		
		$error = "";
		if(!isset($post_array["uploadPassword"]) || !preg_match("/^[A-Za-z0-9]+$/", $post_array["uploadPassword"]) ){
			$error .= "<p class='error message'>Upload Password is a required alphanumeric field.</p> ";
		}
		if(!isset($post_array["downloadPassword"]) || !preg_match("/^[A-Za-z0-9]+$/", $post_array["downloadPassword"]) ){
			$error .= "<p class='error message'>Download Password is a required alphanumeric field.</p> ";
		}	
		return $error;
	}
	
	function savePasswords($post_array){
		$fp = fopen('pw', 'w+');
		fwrite($fp, md5($post_array["uploadPassword"])."\n" );
		fwrite($fp, md5($post_array["downloadPassword"]) );
		fclose($fp);
	}
	
	function getPasswords(){
		if(file_exists(PASSWORD_FILE)){
			$handle = fopen(PASSWORD_FILE, "r");
	        $uploadPassword = trim(fgets($handle, 1024));
	        $downloadPassword = trim(fgets($handle, 1024));
	        
	        return array("uploadPassword"=>$uploadPassword, "downloadPassword"=>$downloadPassword);
		}
		else{
			return array();
		}
	}
}
?>