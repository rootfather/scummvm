/*
 * Relation.c
 * (c) 1993 by Kaweh Kazemi
 * All rights reserved.
 */
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms contained in the LICENSE and
  publiclicensecontract.doc files which should be contained with this
  distribution.
 ****************************************************************************/

#include "clue/data/relation.h"

namespace Clue {

static RELATION nrOfRelations = 1;
static struct relationDef *relationsDefBase = NULL;

int (*CompareKey)(KEY, KEY) = NULL;
KEY(*EncodeKey)(char *) = NULL;
char *(*DecodeKey)(KEY) = NULL;


static struct relationDef *FindRelation(RELATION id) {
	for (struct relationDef *rd = relationsDefBase; rd; rd = rd->rd_next) {
		if (rd->rd_id == id)
			return rd;
	}

	return NULL;
}

RELATION AddRelation(RELATION id) {
	if (!FindRelation(id)) {
		struct relationDef *rd = (struct relationDef *) TCAllocMem(sizeof(*rd), 0);
		if (rd) {
			rd->rd_next = relationsDefBase;
			relationsDefBase = rd;

			rd->rd_id = id;
			nrOfRelations++;

			rd->rd_relationsTable = NULL;

			return rd->rd_id;
		}
	}

	return 0;
}

RELATION CloneRelation(RELATION id, RELATION cloneId) {
	struct relationDef *rd = FindRelation(id);
	if (rd) {
		if (AddRelation(cloneId) || FindRelation(cloneId)) {
			for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next)
				SetP(r->r_leftKey, cloneId, r->r_rightKey, r->r_parameter);

			return cloneId;
		}
	}

	return 0;
}

RELATION RemRelation(RELATION id) {
	struct relationDef **h = &relationsDefBase;

	for (struct relationDef *rd = relationsDefBase; rd; rd = rd->rd_next) {
		if (rd->rd_id == id) {
			*h = rd->rd_next;

			while (rd->rd_relationsTable) {
				struct relation *h2 = rd->rd_relationsTable->r_next;

				TCFreeMem(rd->rd_relationsTable, sizeof(struct relation));
				rd->rd_relationsTable = h2;
			}

			TCFreeMem(rd, sizeof(*rd));

			nrOfRelations--;

			return id;
		}

		h = &rd->rd_next;
	}

	return 0;
}

RELATION SetP(KEY leftKey, RELATION id, KEY rightKey, PARAMETER parameter) {
	struct relationDef *rd = FindRelation(id);

	if (rd && CompareKey) {
		for (struct relation *rel = rd->rd_relationsTable; rel; rel = rel->r_next) {
			if (CompareKey(rel->r_leftKey, leftKey) && CompareKey(rel->r_rightKey, rightKey)) {
				if (parameter != NO_PARAMETER)
					rel->r_parameter = parameter;
				return id;
			}
		}

		struct relation *newRel = (struct relation *) TCAllocMem(sizeof(*newRel), 0);
		if (newRel) {
			newRel->r_next = rd->rd_relationsTable;
			rd->rd_relationsTable = newRel;

			newRel->r_leftKey = leftKey;
			newRel->r_rightKey = rightKey;
			newRel->r_parameter = parameter;

			return id;
		}
	}

	return 0;
}

RELATION UnSet(KEY leftKey, RELATION id, KEY rightKey) {
	struct relationDef *rd = FindRelation(id);

	if (rd && CompareKey) {
		struct relation **h = &rd->rd_relationsTable;
		for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
			if (CompareKey(r->r_leftKey, leftKey)
			        && CompareKey(r->r_rightKey, rightKey)) {
				*h = r->r_next;

				TCFreeMem(r, sizeof(*r));
				return id;
			}

			h = &r->r_next;
		}
	}

	return 0;
}

PARAMETER GetP(KEY leftKey, RELATION id, KEY rightKey) {
	struct relationDef *rd = FindRelation(id);

	if (rd && CompareKey) {
		for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
			if (CompareKey(r->r_leftKey, leftKey)
			        && CompareKey(r->r_rightKey, rightKey))
				return r->r_parameter;
		}
	}

	return NO_PARAMETER;
}

RELATION AskP(KEY leftKey, RELATION id, KEY rightKey, PARAMETER parameter, COMPARSION comparsion) {
	struct relationDef *rd = FindRelation(id);

	if (rd && CompareKey) {
		for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
			if (CompareKey(r->r_leftKey, leftKey)
			        && CompareKey(r->r_rightKey, rightKey)) {
				if (comparsion && (parameter != NO_PARAMETER)) {
					if (comparsion & CMP_EQUAL) {
						if (comparsion & CMP_HIGHER)
							return (r->r_parameter >= parameter ? id : 0);
						else if (comparsion & CMP_LOWER)
							return (r->r_parameter <= parameter ? id : 0);

						return (r->r_parameter == parameter ? id : 0);
					} else if (comparsion & CMP_HIGHER)
						return (r->r_parameter > parameter ? id : 0);
					else if (comparsion & CMP_LOWER)
						return (r->r_parameter < parameter ? id : 0);
					else if (comparsion & CMP_NOT_EQUAL)
						return (r->r_parameter != parameter ? id : 0);

					return 0;
				}

				return id;
			}
		}
	}

	return 0;
}

void AskAll(KEY leftKey, RELATION id, void (*UseKey)(void *)) {
	struct relationDef *rd = FindRelation(id);

	if (rd && CompareKey) {
		for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
			if (CompareKey(r->r_leftKey, leftKey)) {
				if (UseKey)
					UseKey(r->r_rightKey);
			}
		}
	}
}

void UnSetAll(KEY key, void (*UseKey)(KEY)) {
	for (struct relationDef *rd = relationsDefBase; rd; rd = rd->rd_next) {
		struct relation **h = &rd->rd_relationsTable;

		for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
			if (CompareKey(r->r_leftKey, key) || CompareKey(r->r_rightKey, key)) {
				if (UseKey)
					UseKey(key);

				*h = r->r_next;

				TCFreeMem(r, sizeof(*r));
			}

			h = &r->r_next;
		}
	}
}

int SaveRelations(const char *file, uint32 offset, uint32 size, uint16 disk_id) {
	if (relationsDefBase && DecodeKey) {
		FILE *fh = dskOpen(file, "wb");
		if (fh) {
			fprintf(fh, "%s\r\n", REL_FILE_MARK);

			for (struct relationDef *rd = relationsDefBase; rd; rd = rd->rd_next) {
				if (rd->rd_id > offset) {
					if (size && (rd->rd_id > offset + size))
						continue;

					fprintf(fh, "%s\r\n", REL_TABLE_MARK);
					fprintf(fh, "%u\r\n", rd->rd_id);

					for (struct relation *r = rd->rd_relationsTable; r; r = r->r_next) {
						char left[256];
						char right[256];
						strcpy(left, DecodeKey(r->r_leftKey));
						strcpy(right, DecodeKey(r->r_rightKey));
						fprintf(fh, "%s\r\n%s\r\n%u\r\n", left, right, r->r_parameter);
					}
				}
			}

			dskClose(fh);

			return 1;
		}
	}

	return 0;
}

bool LoadRelations(const char *file, uint16 disk_id) {
	char buffer[256];
	char left[256];
	char right[256];

	buffer[0] = '\0';
	left[0] = '\0';
	right[0] = '\0';

	if (EncodeKey) {
		FILE *fh = dskOpen(file, "rb");
		if (fh) {
			dskGetLine(buffer, sizeof(buffer), fh);

			if (strcmp(buffer, REL_FILE_MARK) == 0) {
				dskGetLine(buffer, sizeof(buffer), fh);

				while (!feof(fh) && strcmp(buffer, REL_TABLE_MARK) == 0) {
					RELATION rd;
					fscanf(fh, "%u\r\n", &rd);

					bool goOn = false;
					if (FindRelation(rd))
						goOn = true;
					else if (AddRelation(rd))
						goOn = true;

					if (goOn) {
						while (dskGetLine(left, sizeof(left), fh)) {
							if (strcmp(left, REL_TABLE_MARK) == 0) {
								strcpy(buffer, left);
								break;
							}

							uint32 dummy;
							if (sscanf(left, "%u", &dummy) != 1)
								break;

							dskGetLine(right, sizeof(right), fh);

							if (sscanf(right, "%u", &dummy) != 1)
								break;

							PARAMETER parameter;
							if (fscanf(fh, "%u\r\n", &parameter) != 1)
								break;

							if (!SetP(EncodeKey(left), rd, EncodeKey(right), parameter)) {
								dskClose(fh);
								return false;
							}
						}
					} else {
						dskClose(fh);
						return false;
					}
				}

				dskClose(fh);
				return true;
			}

			dskClose(fh);
		}
	}

	return false;
}

void RemRelations(uint32 offset, uint32 size) {
	struct relationDef *rd_next;
	for (struct relationDef *rd = relationsDefBase; rd; rd = rd_next) {
		rd_next = rd->rd_next;

		if (rd->rd_id > offset) {
			if (size && (rd->rd_id > offset + size)) {
				continue;
			}

			RemRelation(rd->rd_id);
		}
	}
}

} // End of namespace Clue
