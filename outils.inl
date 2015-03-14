/*!
* @fn bool operator==( const s_timeval& tp1, const s_timeval& tp2 )
* @brief Permet comparer le contenue de 2 variables de type s_timeval
*/
inline bool operator==( const s_timeval& tp1, const s_timeval& tp2 )
{
	return ( tp1.tv_sec == tp2.tv_sec && tp1.tv_usec == tp2.tv_usec );
}


/*!
* @fn bool operator>=( const s_timeval& tp1, const s_timeval& tp2 )
* @brief Permet comparer le contenue de 2 variables de type s_timeval
*/
inline bool operator>=( const s_timeval& tp1, const s_timeval& tp2 )
{
	return ( tp1.tv_sec >= tp2.tv_sec && tp1.tv_usec >= tp2.tv_usec );
}


/*!
* @fn bool operator>( const s_timeval& tp1, const s_timeval& tp2 )
* @brief Permet comparer le contenue de 2 variables de type s_timeval
*/
inline bool operator>( const s_timeval& tp1, const s_timeval& tp2 )
{
	return ( tp1.tv_sec > tp2.tv_sec && tp1.tv_usec > tp2.tv_usec );
}


/*!
* @fn bool operator<=( const s_timeval& tp1, const s_timeval& tp2 )
* @brief Permet comparer le contenue de 2 variables de type s_timeval
*/
inline bool operator<=( const s_timeval& tp1, const s_timeval& tp2 )
{
	return ( tp1.tv_sec <= tp2.tv_sec && tp1.tv_usec <= tp2.tv_usec );
}


/*!
* @fn bool operator<( const s_timeval& tp1, const s_timeval& tp2 )
* @brief Permet comparer le contenue de 2 variables de type s_timeval
*/
inline bool operator<( const s_timeval& tp1, const s_timeval& tp2 )
{
	return ( tp1.tv_sec < tp2.tv_sec && tp1.tv_usec < tp2.tv_usec );
}
