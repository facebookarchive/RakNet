<?php
class RecordManager{
	
	//Constructor just creates the RecordFile.data.php
	function RecordManager(){
		if(!file_exists(RECORD_FILE)){
			$fp = fopen(RECORD_FILE, "w");
			fclose($fp);
		}
	}
	
	//Called everytime you run DirectoryServer.php to expire record values.	
	function expireRecords(){
		//IMPORTANT
		//Include the var_export of the variable $records
		include("RecordFile.data.php");
		
		if(!empty($records)){
			foreach($records as $index => $game){
			 	$secs = time() - $game["__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE"];
			 	if($secs > TIMEOUT){
					unset($records[$index]);
			 	}
			}
			
			//Save the file
			$fp = fopen(RECORD_FILE, "w");
			fwrite($fp, '<?php $records = ');
			fwrite($fp, var_export($records, true));
			fwrite($fp, '; ?>');
			fclose($fp);
		}	
	}
	
	function uploadRecords($post_body){
		//IMPORTANT
		//Include the var_export of the variable $records
		include("RecordFile.data.php");
		$output = "";
		$post_array = array();

		//Modified code to handle multiple rows separated by \002
		$post_body_rows = explode("\002", $post_body);
		foreach($post_body_rows as $row_num => $post_body_row){	
			//row columns and values are separated by \001
			//Even indexes are columns, the following odd value is the column_value		
			$post_body_array = explode("\001", $post_body_row);			
			foreach($post_body_array as $index => $post_body_item){
				if($index % 2 == 0){
					$post_array[$row_num][$post_body_item] = $post_body_array[$index+1];	
				}			
			}	
		}						
		
		//This places each value pair into a PHP array to be outputted to the Recordfile
		foreach($post_array as $post_item){
			if(isset($post_item["__GAME_PORT"]) && isset($post_item["__GAME_NAME"]) ){
				$record = array();
				foreach($post_item as $key => $value){				
					//Decode because post values are send with url encoded symbols like %20
					$record[$key] = rawurldecode($value);
				}
				
				//Store the IP address if not included in the POST
				if(!isset($record["__System_Address"])){
					$record["__System_Address"] = $_SERVER["REMOTE_ADDR"];
				}
				
				$record["__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE"] = time();			
				
				//Search the $records for a matching record. If the GAME_PORT, GAME_NAME, and System Address match replace it
				//Otherwise just add the record to the $records array.
				$record_found = false;
				if(!empty($records)){
					foreach($records as $index => $save_record){
						if($save_record["__GAME_PORT"] == $record["__GAME_PORT"]
							&& $save_record["__GAME_NAME"] == $record["__GAME_NAME"] 
							&& $save_record["__System_Address"] == $record["__System_Address"]){
								//We found the record so replace it here.																					
								$records[$index] = $record;
								$record_found = true;
								break;
						}
					}
				}
				
				//Record couldn't be found simply add a new record
				if(!$record_found){
					$records[] = $record;
				}
				
				//Save the file
				$fp = fopen(RECORD_FILE, "w");
				fwrite($fp, '<?php $records = ');
				fwrite($fp, var_export($records, true));
				fwrite($fp, '; ?>');
				fclose($fp);									
			}
			else{ 
				$output .= "\003".microtime(true)."\003";
				$output .= "__GAME_PORT and __GAME_NAME must be provided";
			}						
		}
		return $output;
	}
	
	function downloadRecords(){
		//IMPORTANT
		//Include the var_export of the variable $records
		include("RecordFile.data.php");
		//Comment prefix	
		$output = "\003".microtime(true)."\003"; 
		
		//Output the records. Traverse and output rows separated by \002 and values seperated by \001
		if(!empty($records)){
			$row_count  = 0;			
			foreach($records as $game){
				if(!empty($game)){
					if($row_count > 0){
						$output .= "\002";
					}
					
					$count = 0;				
					foreach($game as $key => $value){
						if($count > 0){
							$output .= "\001";
						}
						$output .= "$key"."\001"."$value";
						$count++;
					}
										
				}
				$row_count++;
			}
		}		
		
		return $output;
	}

	function generateCss(){
		$output = "<html>
		 			<head>
		 			<style type='text/css' media='all'>
		 				.game{
		 					border:1px solid #1E3C64;
		 					background:#4A8AD1;
		 					padding:5px 10px;
		 					color:#EFF7FF;
		 					margin-bottom:5px;
		 				}		 				
		 			</style>
		 			</head>
		 			<body>";
		return $output;
	}
	
	function viewRecords(){
		$output = "";
		
		//IMPORTANT
		//Include the var_export of the variable $records
		include("RecordFile.data.php");
		
		if(!empty($records)){
			foreach($records as $game){
				$vars = "";
				$secs = $game["__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE"] + TIMEOUT - time();
				
				$output .= "<div class='game' >
								<p>Game Port: {$game["__GAME_PORT"]}</p>
								<p>Game Name: {$game["__GAME_NAME"]}</p>
								<p>System Address: {$game["__System_Address"]}</p>
								<p>Time Until Expiration: $secs secs</p>
							";				
				
				foreach($game as $key => $value){
					if($key != "__GAME_PORT" && $key != "__GAME_NAME"
					   && $key != "__System_Address" && $key != "__SEC_AFTER_EPOCH_SINCE_LAST_UPDATE" ){					   	
							$vars .= "<li>".$key.": $value</li>";
					}
					elseif($key == "0"){
						$vars .= "<li>".$key.": $value</li>";
					}
				}
				
				if(!empty($vars)){
					$output .= "<ul>$vars</ul>";
				}
				$output .= "</div>";
			}
		}
		else{
			$output .= "<div class='game' >
						<p>Record Table is Empty.</p>
						</div>";
		}
		$output .= "</body></html>";
		
		return $output;
	}
		
}
?>