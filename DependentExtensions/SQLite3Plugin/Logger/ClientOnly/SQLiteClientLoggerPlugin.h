/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

/// \file
/// \brief Contains utility functions to write logs, which are then sent to a connected instance of SQLite3ServerLoggerPlugin
///


#ifndef ___SQLITE_CLIENT_LOGGER_PLUGIN_H
#define ___SQLITE_CLIENT_LOGGER_PLUGIN_H

#include "SQLite3ClientPlugin.h"
#include "SQLiteLoggerCommon.h"

// return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line);

// If you get Error	2	error C2275: 'RakNet::SQLiteClientLoggerPlugin::ParameterListHelper' : illegal use of this type as an expression
// Either:
// 1. You forgot to write the TABLE_DESCRIPTOR
// 2. You forgot to put the parameter list in parenthesis
#define rakSqlLog(TABLE_DESCRIPTOR, COLUMN_NAMES, PARAMETER_LIST) RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, TABLE_DESCRIPTOR, COLUMN_NAMES, _FILE_AND_LINE_, RakNet::SQLiteClientLoggerPlugin::ParameterListHelper PARAMETER_LIST)
#define rakFnLog(TABLE_DESCRIPTOR, PARAMETER_LIST) RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, TABLE_DESCRIPTOR, 0, _FILE_AND_LINE_, RakNet::SQLiteClientLoggerPlugin::ParameterListHelper PARAMETER_LIST)


namespace RakNet
{
	/// \ingroup SQL_LITE_3_PLUGIN
	/// \brief Result codes for logging
	enum SQLLogResult
	{
		/// OK
		SQLLR_OK,

		/// Didn't instantiate SQLiteClientLoggerPlugin
		SQLLR_NOT_INSTANTIATED,

		/// Didn't call SQLiteClientLoggerPlugin::SetServerParameters
		SQLLR_NO_DATABASE_SET,

		/// String passed for column names unreasonably long (set by MAX_COLUMN_NAMES_LENGTH)
		SQLLR_COLUMN_NAMES_NOT_TERMINATED,

		/// tableName parameter is blank
		SQLLR_TABLE_NAME_BLANK,

		/// Syntax of column names doesn't make sense. Should be column1,column2,column3,...
		SQLLR_TABLE_DESCRIPTOR_FORMAT_INVALID_SYNTAX,

		/// Syntax of column names indicated n columns, but actual number of parameters != n
		SQLLR_TABLE_DESCRIPTOR_FORMAT_WRONG_PARAMETER_COUNT,

		/// Logging when already in a log function
		SQLLR_RECURSION,

		/// Out of memory. See SQLiteClientLoggerPlugin::SetMemoryConstraint
		SQLLR_WOULD_EXCEED_MEMORY_CONSTRAINT
	};

	/// \brief Contains utility functions to write logs, which are then sent to a connected instance of SQLite3ServerLoggerPlugin
	/// \details Connect with RakPeerInterface or PacketizedTCP first, then use the sqlLog or functionLog to write logs.
	/// Don't use this class directly, except to call SetServerParameters()
	/// \ingroup SQL_LITE_3_PLUGIN
	class RAK_DLL_EXPORT SQLiteClientLoggerPlugin : public SQLite3ClientPlugin
	{
	public:
		SQLiteClientLoggerPlugin();
		virtual ~SQLiteClientLoggerPlugin();

		/// Required to use the system. Call SetServerParameters() before calling sqlLog() or functionLog()
		/// systemAddress Address of the server we are already connected to
		/// _dbIdentifier session identifier. If the server is using CREATE_SHARED_NAMED_DB_HANDLE or CREATE_EACH_NAMED_DB_HANDLE then this is the name of the created file. Otherwise, it is the dbIdentifier passed to SQLite3ServerPlugin::AddDBHandle();
		void SetServerParameters(const SystemAddress &systemAddress, RakNet::RakString _dbIdentifier);

		/// Every entry in the table has a tick count
		/// Call this function to increment it by one
		void IncrementAutoTickCount(void);

		/// If the amount of data buffered to go out on TCP exceeds this amount, then the log is aborted rather than sent
		/// This is only used when sending through TCP
		/// \param[in] constraint Use 0 for unlimited. Otherwise specify the amount in bytes
		void SetMemoryConstraint(unsigned int constraint);

		// ---------------------------------- INTERNAL -------------------------------
		SQLLogResult CheckQuery(bool isFunction, const char *tableName, const char *columnNames, int numParameters);

		struct ParameterListHelper
		{
			ParameterListHelper() : paramCount(0) {}

			template <class T1>
			ParameterListHelper(const T1 &t1) : p0(t1), paramCount(1) {}

			template <class T1, class T2>
			ParameterListHelper(const T1 &t1, const T2 &t2) : p0(t1), p1(t2), paramCount(2) {}

			template <class T1, class T2, class T3>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3) : p0(t1), p1(t2), p2(t3), paramCount(3) {}

			template <class T1, class T2, class T3, class T4>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4) : p0(t1), p1(t2), p2(t3), p3(t4), paramCount(4) {}

			template <class T1, class T2, class T3, class T4, class T5>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), paramCount(5) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), paramCount(6) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), paramCount(7) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), paramCount(8) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), paramCount(9) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), paramCount(10) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), p10(t11), paramCount(11) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), p10(t11), p11(t12), paramCount(12) {}
			
			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), p10(t11), p11(t12), p12(t13), paramCount(13) {}
			
			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), p10(t11), p11(t12), p12(t13), p13(t14), paramCount(14) {}

			template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
			ParameterListHelper(const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15) : p0(t1), p1(t2), p2(t3), p3(t4), p4(t5), p5(t6), p6(t7), p7(t8), p8(t9), p9(t10), p10(t11), p11(t12), p12(t13), p13(t14), p14(t15), paramCount(15) {}

			// Array doesn't work - no constructor initialization
			//const LogParameter parms[12];
			const LogParameter p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14;
			const int paramCount;
		};


		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const ParameterListHelper &parameterList );

		/*
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11 );
		SQLLogResult SqlLog( bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, const LogParameter *p1, const LogParameter *p2, const LogParameter *p3, const LogParameter *p4, const LogParameter *p5, const LogParameter *p6, const LogParameter *p7, const LogParameter *p8, const LogParameter *p9, const LogParameter *p10, const LogParameter *p11, const LogParameter *p12  );
		*/

		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const ParameterListHelper &parameterList )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,parameterList.paramCount); if (r!=SQLLR_OK) return r;
			r = logger->SqlLog(isFunction, tableName, columnNames, file, line, parameterList);
			return r;
		}

		/*
		static RakNet::SQLLogResult __sqlLogInternal( bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line )
		{
			if (logger==0) return RakNet::SQLLR_NOT_INSTANTIATED;
			RakNet::SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,0); if (r!=RakNet::SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line);
		}
		template <class T1>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,1); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				);
		}
		template <class T1, class T2>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,2); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				);
		}
		template <class T1, class T2, class T3>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,3); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				);
		}
		template <class T1, class T2, class T3, class T4>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,4); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,5); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,6); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,7); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,8); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				,&LogParameter(arg8)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,9); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				,&LogParameter(arg8)
				,&LogParameter(arg9)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,10); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				,&LogParameter(arg8)
				,&LogParameter(arg9)
				,&LogParameter(arg10)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,11); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				,&LogParameter(arg8)
				,&LogParameter(arg9)
				,&LogParameter(arg10)
				,&LogParameter(arg11)
				);
		}
		template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
		static RakNet::SQLLogResult __sqlLogInternal(bool isFunction, const char *tableName, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11, const T12 &arg12 )
		{
			if (logger==0) return SQLLR_NOT_INSTANTIATED;
			SQLLogResult r = logger->CheckQuery(isFunction, tableName, columnNames,12); if (r!=SQLLR_OK) return r;
			return logger->SqlLog(isFunction, tableName, columnNames, file, line
				,&LogParameter(arg1)
				,&LogParameter(arg2)
				,&LogParameter(arg3)
				,&LogParameter(arg4)
				,&LogParameter(arg5)
				,&LogParameter(arg6)
				,&LogParameter(arg7)
				,&LogParameter(arg8)
				,&LogParameter(arg9)
				,&LogParameter(arg10)
				,&LogParameter(arg11)
				,&LogParameter(arg12)
				);
		}
		*/

		virtual void Update(void);

		static SQLiteClientLoggerPlugin* logger;


	protected:
		void SerializeHeader(RakNet::BitStream *bitStream, bool isFunctionCall, const char *tableName, const char *columnNames, const char *file, const int line, unsigned char parameterCount ) const;

		SystemAddress serverAddress;
		RakNet::RakString dbIdentifier;
		uint32_t tickCount;
		bool recursiveCheck;
		unsigned int memoryConstraint;
	};
}

/*
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line);
}
template <class T1>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1);
}
template <class T1, class T2>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2);
}
template <class T1, class T2, class T3>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3);
}
template <class T1, class T2, class T3, class T4>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4);
}
template <class T1, class T2, class T3, class T4, class T5>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5);
}
template <class T1, class T2, class T3, class T4, class T5, class T6>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
RakNet::SQLLogResult __sqlLog(const char *tableDescriptor, const char *columnNames, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11, const T12 &arg12 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(false, tableDescriptor, columnNames, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
// TODO - Add _FILE_AND_LINE_ automatically somehow or this is nearly useless
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line);
}
template <class T1>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1);
}
template <class T1, class T2>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2);
}
template <class T1, class T2, class T3>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3);
}
template <class T1, class T2, class T3, class T4>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4);
}
template <class T1, class T2, class T3, class T4, class T5>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5);
}
template <class T1, class T2, class T3, class T4, class T5, class T6>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
RakNet::SQLLogResult __functionLog(const char *functionName, const char *file, const int line, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8, const T9 &arg9, const T10 &arg10, const T11 &arg11, const T12 &arg12 )
{
	return RakNet::SQLiteClientLoggerPlugin::__sqlLogInternal(true, functionName, 0, file, line, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
}
*/
#endif
