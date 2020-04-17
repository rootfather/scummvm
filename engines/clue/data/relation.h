/*
 * Relation.h
 * (c) 1993 by Kaweh Kazemi
 * All rights reserved.
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#ifndef MODULE_RELATION
#define MODULE_RELATION

//#include <limits.h>
#include "clue/memory/memory.h"
#include "database.h"


namespace Clue {

#define RELATION           uint32
#define PARAMETER          uint32
#define COMPARSION         uint8

#define CMP_NO             ((COMPARSION)0)
#define CMP_EQUAL          ((COMPARSION)(1))
#define CMP_NOT_EQUAL      ((COMPARSION)(1 << 1))
#define CMP_HIGHER         ((COMPARSION)(1 << 2))
#define CMP_LOWER          ((COMPARSION)(1 << 3))

#define REL_FILE_MARK      "RELF"
#define REL_TABLE_MARK     "RTAB"

#define NO_PARAMETER       UINT_MAX /* limits.h */


#define Set(leftKey,id,rightKey)  SetP (leftKey, id, rightKey, NO_PARAMETER)
#define Ask(leftKey,id,rightKey)  AskP (leftKey, id, rightKey, NO_PARAMETER, CMP_NO)


struct relationDef {
	struct relationDef *rd_next;

	RELATION rd_id;
	struct relation *rd_relationsTable;
};

struct relation {
	struct relation *r_next;

	dbObjectNode *r_leftKey;
	dbObjectNode *r_rightKey;
	PARAMETER r_parameter;
};


extern int (*CompareKey)(dbObjectNode *, dbObjectNode *);

RELATION AddRelation(RELATION);
RELATION CloneRelation(RELATION, RELATION);
RELATION RemRelation(RELATION);

RELATION SetP(dbObjectNode *, RELATION, dbObjectNode *, PARAMETER);
RELATION UnSet(dbObjectNode *, RELATION, dbObjectNode *);
PARAMETER GetP(dbObjectNode *, RELATION, dbObjectNode *);
RELATION AskP(dbObjectNode *, RELATION, dbObjectNode *, PARAMETER, COMPARSION);
void AskAll(dbObjectNode *, RELATION, void (*)(dbObjectNode *));

int SaveRelations(const char *, uint32, uint32, uint16 disk_id);
bool LoadRelations(const char *, uint16 disk_id);
void RemRelations(uint32, uint32);

#if 0
void UnSetAll(KEY, void (*)(KEY));
#endif
} // End of namespace Clue

#endif
