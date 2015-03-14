<?php
// Multi-tache : http://php.net/manual/en/function.pcntl-fork.php

if( $argc >= 2 ){
	if( $argv[1] === 'help' || $argv[1] === '/help' || $argv[1] === '-help' || $argv[1] === '--help' || $argv[1] === '-h' )
		exit('Utilisation:'."\r\n".'php '.$argv[0].' [full [force]]'."\r\n\t".'full:'."\t".'Permet d\'avoir la transcription de toute la backtrace'."\r\n\t".'force:'."\t".'Permet d\'avoir la transcription de toute la backtrace même si celle-ci a une taille >= 100Mo');

	if( !isInArgList('full') && !isInArgList('force') )
		exit('Parametres incorrect!'."\r\n".'Utilisation:'."\r\n".'php '.$argv[0].' [full [force]]'."\r\n\t".'full:'."\t".'Permet d\'avoir la transcription de toute la backtrace'."\r\n\t".'force:'."\t".'Permet d\'avoir la transcription de toute la backtrace même si celle-ci a une taille >= 100Mo');
}


//*******************************************************************************
// Ajustement des paramètres
echo '> Ajustement des parametres: ',"\r\n"
,"\t",'memory_limit',"\t\t",'= ',ini_set('memory_limit', '3000M'),' => ',ini_get('memory_limit'),"\r\n"
,"\t",'max_execution_time',"\t",'= ',ini_set('max_execution_time', 0),' => ',ini_get('max_execution_time'),"\r\n";
$separator = '/';
if( IsSet($_SERVER['SHELL']) ){// Linux
	$separator = '/';
}elseif( IsSet($_SERVER['OS']) ){// Windows
	$separator = '\\';
}


//*******************************************************************************
// Obtention des symboles et des adresses associées ( Pack: "ADDR T ma_fonction" )
echo '> nm -C bomberman.exe';
exec('nm -C ..'.$separator.'bomberman.exe', $fp_sym);
$fp_sym = implode("\n", $fp_sym);
echo "\t\t\t\t\t\tOK\r\n";


//*******************************************************************************
// On dépack les données
echo '> preg_match_all adresses -> symboles';
preg_match_all('`([0-9A-Za-z]+) [A-Za-z0-9] ([0-9A-Za-z_:@\(\)\*, <>&\~=\!\+\-\]\[]+)\n`', $fp_sym, $SymToAddr);
unset($fp_sym);
echo "\t\t\t\tOK\r\n";


//*******************************************************************************
// Obtention des données :
// - Nombre de fois qu'une adresse apparait ( permet de détecter une fuite mémoire )
// - Profondeur (Permet de savoir) combien de fonction on été appelé pour en arrivé là où on en est
$count = array();
$depth = array();
$out = 0;
$nb_data = 0;
$temps_rafraichissement = time();
$temps_1percent = 0;
$LOG_TYPE = 0;
$PREG = 0;
$fp_sym = 0;
$out2 = 0;
if( ($fp = fopen('backtrace.log', 'r')) ){
	// Permet de calculer la durée des calcules
	fseek($fp, 0, SEEK_END);// On se met a la fin du fichier
	$nb_data = ftell($fp);// Pour obtenir le nombre de bytes
	rewind($fp);// Remise à 0 de fp

	$tmp = fgets($fp);
	if( $tmp{0} === '-' ){// Old log system
		$LOG_TYPE ='old';
		$PREG = '`([ ]*)<?->? ([A-Za-z0-9]+)`';
		rewind($fp);// Remise à 0 de fp
	}elseif( $tmp{0} === '[' && $tmp{1} === 'S' ){// New log system ( Spécifique )
		if( $tmp{2} === 'U' )
			$LOG_TYPE = 'Specific UNIX';
		else
			$LOG_TYPE = 'Specific';
		$PREG = '`\[([0-9]+)\] *([A-Za-z0-9]+)`';// [0] 004018BB
	}elseif( $tmp{0} === '[' && $tmp{1} === 'F' ){// New log system ( Full <=> Old log system )
		$LOG_TYPE = 'FULL';
		$PREG = '`([ ]*)<?->? ([A-Za-z0-9]+)`';
	}else{// Inconnu
		echo 'Format de log inconnu',"\r\n";
		exit;
	}
	unset($tmp);

	echo '> Lecture et traitement des donnees',"\t\t\t\t...\r\n";
	$temps_1percent = microtime(true);
	while( !feof($fp) )
	{
		// On lit la ligne courante et on parsse le tout
		if( preg_match_all($PREG, fgets($fp), $out, PREG_SET_ORDER) ){
			if( !IsSet($count[$out[0][2]]) ){
				$count[$out[0][2]] = 1;
				$pos = isInList( $SymToAddr[1], $out[0][2] );
				if( $pos === false ){
					exec('addr2line -pfCe ..'.$separator.'bomberman.exe '.$out[0][2], $fp_sym);
					// Les retours possibles:
					// partie at /home/ph3w/lif7/partie.cpp:24
					// partie::partie() at ??:0
					if( preg_match_all('`([0-9A-Za-z_:@\(\)\*, <>&\~=\!\+\-\]\[]+) at .*/([A-Za-z0-9_\.\? ]+):([0-9]+)`', $fp_sym[0], $out2, PREG_SET_ORDER) ){
						$SymToAddr[1][] = $out[0][2];
						if( $out2[0][2] !== '?' )
							$SymToAddr[2][] = $out2[0][1].' at <'.$out2[0][2].':'.$out2[0][3].'>';
						else
							$SymToAddr[2][] = $out2[0][1];
					}elseif( preg_match_all('`([0-9A-Za-z_:@\(\)\*, <>&\~=\!\+\-\]\[]+) at`', $fp_sym[0], $out2, PREG_SET_ORDER) ){
						$SymToAddr[1][] = $out[0][2];
						$SymToAddr[2][] = $out2[0][1];
					}
					unset($fp_sym);
				}else{
					exec('addr2line -pfCe ..'.$separator.'bomberman.exe '.$out[0][2], $fp_sym);
					// Les retours possibles:
					// partie at /home/ph3w/lif7/partie.cpp:24
					// partie::partie() at ??:0
					if( preg_match_all('`([0-9A-Za-z_:@\(\)\*, <>&\~=\!\+\-\]\[]+) at .*/([A-Za-z0-9_\.\? ]+):([0-9]+)`', $fp_sym[0], $out2, PREG_SET_ORDER) ){
						if( $out2[0][2] !== '?' )
							$SymToAddr[2][$pos] = $out2[0][1].' at <'.$out2[0][2].':'.$out2[0][3].'>';
					}elseif( preg_match_all('`([0-9A-Za-z_:@\(\)\*, <>&\~=\!\+\-\]\[]+) at`', $fp_sym[0], $out2, PREG_SET_ORDER) ){
						$SymToAddr[2][$pos] = $out2[0][1];
					}
					unset($fp_sym);
				}
			}else
				$count[$out[0][2]]++;

			if( $LOG_TYPE === 'Specific' || $LOG_TYPE === 'Specific UNIX' ){
				$depth[$out[0][2]] = $out[0][1];
			}else{
				$depth[$out[0][2]] = strlen($out[0][1]);
			}

			if( !$temps_1percent && $temps_rafraichissement <= time() ){
				echo '>> Avancement: ',round(ftell($fp)*100/$nb_data, 3),'%',"\r\n";
				$temps_rafraichissement = time()+1;
			}
		}

		if( $temps_1percent && round(ftell($fp)*100/$nb_data, 3) >= 1.0 ){
			$temps_1percent = ceil((microtime(true)-$temps_1percent)*100);
			if( $temps_1percent >= 3600.0 ){// 1h
				$temps_1percent = round($temps_1percent/3600.0, 2);
				$temps_1percent = explode('h', number_format($temps_1percent, 2, 'h', ''));
				if( $temps_1percent[1] >= 60 ){
					$temps_1percent[1] /= 60;
					$temps_1percent[1] = round($temps_1percent[1], 2);
					$temps_1percent[0]++;
				}
				$temps_1percent = str_replace('h', 'heure(s)',implode('h', $temps_1percent)).'min(s)';
			}elseif( $temps_1percent >= 60.0 ){
				$temps_1percent = round($temps_1percent/60.0, 2);
				$temps_1percent = explode('m', number_format($temps_1percent, 2, 'm', ''));
				if( $temps_1percent[1] >= 60 ){
					$temps_1percent[1] /= 60;
					$temps_1percent[1] = round($temps_1percent[1], 2);
					$temps_1percent[0]++;
				}
				$temps_1percent = str_replace('m', 'min(s)', implode('m', $temps_1percent)).'sec(s)';
			}else{
				$temps_1percent = $temps_1percent.'sec(s)';
			}
			echo '-----> Temps estime: ',$temps_1percent,' <-----',"\r\n";
			$temps_1percent = 0;
		}
	}
	echo '# Lecture et traitement des donnees',"\t\t\t\tOK\r\n";
	fclose($fp);
}else{
	exit('Erreur lors de l\'ouverture du fichier backtrace.log');
}
unset($out);


//*******************************************************************************
// Analyse des données pour déterminer la pile d'execution/d'appels
// Si le nombre d'appel a une fonction est impaire => On est dedans ( car in & out => manque out !)
echo '> Analyse pour determiner la pile d\'appels (call stack)';
$find = array();
foreach( $count as $key => $val )
	if( $val%2 )
		$find[$key] = $depth[$key];
echo "\t\tOK\r\n";


// Ouverture du fichier qui contiendra les résultats
if( !($fp = fopen('backtrace_translate.log', 'w')) )
	exit('Erreur lors de l\'ouverture du fichier <backtrace_translate.log>');


//*******************************************************************************
// Enregistrement de la pile d'exécution/d'appels (call stack)
echo '> Enregistrement de la pile d\'appels (call stack)';
fputs($fp, '---------------------------------------------------------------------------------'."\r\n");
fputs($fp, '---------------------------------- CALL STACK -----------------------------------'."\r\n\r\n");
$i=0;
foreach( $find as $key => $val )
	fputs($fp, '['.$i++.'] '.str_replace($SymToAddr[1], $SymToAddr[2], strtolower($key))."\r\n");

if( count($find) ){
	fseek($fp, -2, SEEK_CUR);// Pour effacer le dernier \r\n
	fputs($fp, " <-- Crash Here !\r\n\r\n");
}else{
	fputs($fp, 'NO CRASH ! Be happy !'."\r\n\r\n");
}
echo "\t\tOK\r\n";


//*******************************************************************************
// Enregistrement des XXX fonctions les plus utilisées
// ( On compte le nombre d'appel )
$nb_functions = 15;
echo '> Enregistrement des ',$nb_functions,' fonctions les plus appelees';
if( $LOG_TYPE !== 'Specific' && $LOG_TYPE !== 'Specific UNIX' ){
	fputs($fp, "\r\n".'---------------------------------------------------------------------------------'."\r\n");
	fputs($fp, '------------------------- THE '.$nb_functions.' MOST CALLED FUNCTIONS --------------------------'."\r\n\r\n");
	arsort($count);
	foreach( $count as $key => $val )
	{
		fputs($fp, '[Nombre d\'appel: '.$val.'] '.str_replace($SymToAddr[1], $SymToAddr[2], strtolower($key))."\r\n");

		if( !--$nb_functions )
			break;
	}
	fputs($fp, "\r\n");
	echo "\t\tOK\r\n";
}else{
	echo "\t\tPASS\r\n";
}


//*******************************************************************************
// On enregsitre toutes les actions avec la pile d'appels dans le fichier final
// On convertit toutes les adresses au symbole / au nom de fonction qui lui appartient
if( isInArgList('full') && $LOG_TYPE !== 'Specific' && $LOG_TYPE !== 'Specific UNIX' ){
	echo '> Enregistrement de toute la pile d\'appels (adresse -> sym)',"\t",'...',"\r\n";
	$buffer = '';
	$buffer_size = 0;
	$max_buffer_size = 2000;// Nombre de lignes a bufferiser

	fputs($fp, "\r\n".'---------------------------------------------------------------------------------'."\r\n");
	fputs($fp, '-------------------------------- FULL BACKTRACE ---------------------------------'."\r\n\r\n");
	if( filesize('backtrace.log')/1024/1024 <= 100 || isInArgList('force') ){
		if( ($fp_read = fopen('backtrace.log', 'r')) ){

			$temps_1percent = microtime(true);
			$tmp = 3;
			while( !feof($fp_read) ){
				if( $buffer_size < $max_buffer_size ){// Bufferisation
					$buffer .= fgets($fp_read);
					$buffer_size++;
				}else{// Ecriture des données
					fputs($fp, str_replace($SymToAddr[1], $SymToAddr[2], strtolower($buffer), $tmp));// On lit la ligne courante
					if( $tmp !== $max_buffer_size ){// Si on a pas le même nombre de remplacement que de ligne ( et donc que d'adresse )
						$tmp = explode("\r\n", str_replace($SymToAddr[1], $SymToAddr[2], strtolower($buffer)));
						$tmp2 = explode("\r\n", strtolower($buffer));
						for( $i=count($tmp)-1; $i>=0; $i-- )// Alors on parcours les derniers résultat pour trouver l'adresse problématique.
							if( $tmp[$i] === $tmp2[$i] ){
								file_put_contents( 'backtrace_translate_ERROR.log', $tmp[$i], FILE_APPEND );// On log le tout
								$i=-1;
							}
						unset($tmp, $tmp2);// On libère la mémoire
					}
					$buffer = '';
					$buffer_size = 0;
				}

				// Affichage de l'avancement
				if( $temps_rafraichissement <= time() ){
					echo '>> Avancement: ',round(ftell($fp_read)*100/$nb_data, 3),'%',"\r\n";
					$temps_rafraichissement = time()+1;
				}

				// Affichage du temps estimé
				if( $temps_1percent && round(ftell($fp_read)*100/$nb_data, 3) >= 1.0 ){
					$temps_1percent = ceil((microtime(true)-$temps_1percent)*100);
					if( $temps_1percent >= 3600.0 ){// 1h
						$temps_1percent = round($temps_1percent/3600.0, 2);
						$temps_1percent = explode('h', number_format($temps_1percent, 2, 'h', ''));
						if( $temps_1percent[1] >= 60 ){
							$temps_1percent[1] /= 60;
							$temps_1percent[1] = round($temps_1percent[1], 2);
							$temps_1percent[0]++;
						}
						$temps_1percent = str_replace('h', 'heure(s)',implode('h', $temps_1percent)).'min(s)';
					}elseif( $temps_1percent >= 60.0 ){
						$temps_1percent = round($temps_1percent/60.0, 2);
						$temps_1percent = explode('m', number_format($temps_1percent, 2, 'm', ''));
						if( $temps_1percent[1] >= 60 ){
							$temps_1percent[1] /= 60;
							$temps_1percent[1] = round($temps_1percent[1], 2);
							$temps_1percent[0]++;
						}
						$temps_1percent = str_replace('m', 'min(s)', implode('m', $temps_1percent)).'sec(s)';
					}else{
						$temps_1percent = $temps_1percent.'sec(s)';
					}
					echo '-----> Temps estime: ',$temps_1percent,' <-----',"\r\n";
					$temps_1percent = 0;
				}

			}
			// Si le buffer n'est pas vide => On le vide
			if( $buffer_size )
				fputs($fp, str_replace($SymToAddr[1], $SymToAddr[2], strtolower($buffer)));// On lit la ligne courante

			// On ferme le fichier
			fclose($fp_read);

			unset($buffer, $buffer_size, $fp_read, $max_buffer_size);

			echo '# Enregistrement de toute la pile d\'appels (adresse -> sym)',"\tOK\r\n";
		}else{
			fputs($fp, 'Unable to open the file <backtrace.log>');
			echo '# Enregistrement de toute la pile d\'appels (adresse -> sym)',"\tERR\r\n";
		}
	}else{
		fputs($fp, 'Too many data ! (use the arg "force" if you realy want it)');
		echo '# Enregistrement de toute la pile d\'appels (adresse -> sym)',"\tPASS\r\n";
	}
}else{
	echo '> Enregistrement de toute la pile d\'appels (adresse -> sym)',"\tPASS\r\n";
}

fclose($fp);


//*******************************************************************************
// Cette fonction permet de dire ( true / false ) si $arg est dans la liste des
// arguments passés au script
function isInArgList( $arg )
{
	return $_SERVER['argc'] >= 2 && in_array($arg, $_SERVER['argv']);
}

function isInList( $lst, $ptr )
{
	$size = count($lst);
	for( $i=0; $i>$size; $i++ )
		if( $lst[$i] === $ptr )
			return $i;
	return false;
}
?>