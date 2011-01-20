template<class Type>
inline unsigned int QList<Type>::size()
{
	return c_nb_ojb;
}


template<class Type>
inline Type& QList<Type>::operator[](unsigned int n)
{
	return c_list[n];
}


template<class Type>
QList<Type>::QList()
{
	c_list = 0;
	c_nb_ojb = 0;
}


template<class Type>
QList<Type>::QList( unsigned int nb, Type& fill )
{
	c_nb_ojb = nb;
	c_list = new Type[nb];
	for( unsigned int i=0; i<nb; i++ )
		c_list[i] = fill;
}


template<class Type>
QList<Type>::~QList()
{
	if( c_list && c_nb_ojb )
		delete[] c_list;

	c_list = 0;
	c_nb_ojb = 0;
}


template<class Type>
void QList<Type>::resize( unsigned int size, Type& fill )
{
	if( c_list )
		delete[] c_list;

	c_nb_ojb = size;
	c_list = new Type[size];
	for( unsigned int i=0; i<size; i++ )
		c_list[i] = fill;
}


template<class Type>
Type& QList<Type>::at(unsigned int n)
{
	if( n >= c_nb_ojb )
		stdErrorE("Tentative d'accès a une case mémoire non existante ! Cases mémoires disponible [0-%u[ et demande d'accès à %u", c_nb_ojb, n);

	return c_list[n];
}


template<class Type>
void QList<Type>::operator<<( const Type& newObj )
{
	if( c_list ){
		Type* tmp = new Type[c_nb_ojb+1];
		for( unsigned int i=0; i<c_nb_ojb; i++ )
			tmp[i] = c_list[i];

		delete[] c_list;
		c_list = tmp;
		c_list[c_nb_ojb] = newObj;
		c_nb_ojb++;

	}else{
		c_list = new Type[1];
		c_list[0] = newObj;
		c_nb_ojb = 1;
	}
}


template<class Type>
void QList<Type>::insert( unsigned int at, const Type& newObj )
{
	if( at >= c_nb_ojb ){
		stdError("Attention ! insert impossible ! Insert à %u et case disponible [0-%u[", at, c_nb_ojb);
		this->operator <<(newObj);
		return ;
	}


	Type* tmp = new Type[c_nb_ojb+1];
	for( unsigned int i=0; i<at; i++ )
		tmp[i] = c_list[i];

	tmp[at] = newObj;

	for( unsigned int i=at; i<c_nb_ojb; i++ )
		tmp[i+1] = c_list[i];

	delete[] c_list;
	c_list = tmp;
	c_nb_ojb++;
}


template<class Type>
void QList<Type>::remove( unsigned int at )
{
	if( at >= c_nb_ojb )
		stdErrorE("Attention ! remove impossible ! Remove à %u et case disponible [0-%u[", at, c_nb_ojb);

	Type* tmp = new Type[c_nb_ojb-1];
	for( unsigned int i=0; i<at; i++ )
		tmp[i] = c_list[i];

	for( unsigned int i=at+1; i<c_nb_ojb; i++ )
		tmp[i-1] = c_list[i];

	delete[] c_list;
	c_list = tmp;
	c_nb_ojb--;
}
