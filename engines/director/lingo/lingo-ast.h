/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef DIRECTOR_LINGO_LINGO_AST_H
#define DIRECTOR_LINGO_LINGO_AST_H

namespace Director {

struct Node;
struct ScriptNode;
struct FactoryNode;
struct HandlerNode;
struct CmdNode;
struct GlobalNode;
struct PropertyNode;
struct InstanceNode;
struct IntNode;
struct FloatNode;
struct SymbolNode;
struct StringNode;
struct FuncNode;
struct VarNode;
struct ParensNode;
struct UnaryOpNode;
struct BinaryOpNode;

typedef Common::Array<Node *> NodeList;
typedef Common::Array<Common::String *> IDList;

typedef void (*inst)(void);

template <typename T>
void deleteList(Common::Array<T> *list) {
	for (uint i = 0; i < list->size(); i++) {
		delete (*list)[i];
	}
	delete list;
}

enum NodeType {
	kScriptNode,
	kFactoryNode,
	kHandlerNode,
	kCmdNode,
	kGlobalNode,
	kPropertyNode,
	kInstanceNode,
	kIntNode,
	kFloatNode,
	kSymbolNode,
	kStringNode,
	kFuncNode,
	kVarNode,
	kParensNode,
	kUnaryOpNode,
	kBinaryOpNode
};

/* NodeVisitor */

class NodeVisitor {
public:
	NodeVisitor() {}
	virtual ~NodeVisitor() {}

	virtual void visitScriptNode(ScriptNode *node) = 0;
	virtual void visitFactoryNode(FactoryNode *node) = 0;
	virtual void visitHandlerNode(HandlerNode *node) = 0;
	virtual void visitCmdNode(CmdNode *node) = 0;
	virtual void visitGlobalNode(GlobalNode *node) = 0;
	virtual void visitPropertyNode(PropertyNode *node) = 0;
	virtual void visitInstanceNode(InstanceNode *node) = 0;
	virtual void visitIntNode(IntNode *node) = 0;
	virtual void visitFloatNode(FloatNode *node) = 0;
	virtual void visitSymbolNode(SymbolNode *node) = 0;
	virtual void visitStringNode(StringNode *node) = 0;
	virtual void visitFuncNode(FuncNode *node) = 0;
	virtual void visitVarNode(VarNode *node) = 0;
	virtual void visitParensNode(ParensNode *node) = 0;
	virtual void visitUnaryOpNode(UnaryOpNode *node) = 0;
	virtual void visitBinaryOpNode(BinaryOpNode *node) = 0;
};

/* Node */

struct Node {
	NodeType type;
	bool isExpression;
	bool isStatement;

	Node(NodeType t) : type(t), isExpression(false), isStatement(false) {}
	virtual ~Node() {}
	virtual void accept(NodeVisitor *visitor) = 0;
};

/* ExprNode */

struct ExprNode : Node {
	ExprNode(NodeType t) : Node(t) {
		isExpression = true;
	}
	virtual ~ExprNode() {}
};

/* StmtNode */

struct StmtNode : Node {
	StmtNode(NodeType t) : Node(t) {
		isStatement = true;
	}
	virtual ~StmtNode() {}
};

/* ScriptNode */

struct ScriptNode : Node {
	NodeList *nodes;

	ScriptNode(NodeList *nodesIn): Node(kScriptNode), nodes(nodesIn) {}
	virtual ~ScriptNode() {
		deleteList(nodes);
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitScriptNode(this);
	}
};

/* FactoryNode */

struct FactoryNode : Node {
	Common::String *name;
	NodeList *methods;

	FactoryNode(Common::String *nameIn, NodeList *methodsIn)
		: Node(kFactoryNode), name(nameIn), methods(methodsIn) {}
	virtual ~FactoryNode() {
		delete name;
		deleteList(methods);
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitFactoryNode(this);
	}
};

/* HandlerNode */

struct HandlerNode : Node {
	Common::String *name;
	IDList *args;
	NodeList *stmts;

	HandlerNode(Common::String *nameIn, IDList *argsIn, NodeList *stmtsIn)
		: Node(kHandlerNode), name(nameIn), args(argsIn), stmts(stmtsIn) {}
	virtual ~HandlerNode() {
		delete name;
		deleteList(args);
		deleteList(stmts);
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitHandlerNode(this);
	}
};

/* CmdNode */

struct CmdNode : StmtNode {
	Common::String *name;
	NodeList *args;

	CmdNode(Common::String *nameIn, NodeList *argsIn)
		: StmtNode(kCmdNode), name(nameIn), args(argsIn) {}
	virtual ~CmdNode() {
		delete name;
		deleteList(args);
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitCmdNode(this);
	}
};

/* GlobalNode */

struct GlobalNode : StmtNode {
	IDList *names;

	GlobalNode(IDList *namesIn) : StmtNode(kGlobalNode), names(namesIn) {}
	virtual ~GlobalNode() {
		delete names;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitGlobalNode(this);
	}
};

/* PropertyNode */

struct PropertyNode : StmtNode {
	IDList *names;

	PropertyNode(IDList *namesIn) : StmtNode(kPropertyNode), names(namesIn) {}
	virtual ~PropertyNode() {
		delete names;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitPropertyNode(this);
	}
};

/* InstanceNode */

struct InstanceNode : StmtNode {
	IDList *names;

	InstanceNode(IDList *namesIn) : StmtNode(kInstanceNode), names(namesIn) {}
	virtual ~InstanceNode() {
		delete names;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitInstanceNode(this);
	}
};

/* IntNode */

struct IntNode : ExprNode {
	int val;

	IntNode(int valIn) : ExprNode(kIntNode), val(valIn) {}
	virtual ~IntNode() = default;
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitIntNode(this);
	}
};

/* FloatNode */

struct FloatNode : ExprNode {
	double val;

	FloatNode(double valIn) : ExprNode(kFloatNode), val(valIn) {}
	virtual ~FloatNode() = default;
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitFloatNode(this);
	}
};

/* SymbolNode */

struct SymbolNode : ExprNode {
	Common::String *val;

	SymbolNode(Common::String *valIn) : ExprNode(kSymbolNode), val(valIn) {}
	virtual ~SymbolNode() {
		delete val;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitSymbolNode(this);
	}
};

/* StringNode */

struct StringNode : ExprNode {
	Common::String *val;

	StringNode(Common::String *valIn) : ExprNode(kStringNode), val(valIn) {}
	virtual ~StringNode() {
		delete val;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitStringNode(this);
	}
};

/* FuncNode */

struct FuncNode : StmtNode {
	Common::String *name;
	NodeList *args;

	FuncNode(Common::String *nameIn, NodeList *argsIn)
		: StmtNode(kFuncNode), name(nameIn), args(argsIn) {}
	virtual ~FuncNode() {
		delete name;
		deleteList(args);
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitFuncNode(this);
	}
};

/* VarNode */

struct VarNode : ExprNode {
	Common::String *name;

	VarNode(Common::String *nameIn) : ExprNode(kVarNode), name(nameIn) {}
	virtual ~VarNode() {
		delete name;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitVarNode(this);
	}
};

/* ParensNode */

struct ParensNode : ExprNode {
	Node *expr;

	ParensNode(Node *exprIn) : ExprNode(kParensNode), expr(exprIn) {}
	virtual ~ParensNode() {
		delete expr;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitParensNode(this);
	}
};

/* UnaryOpNode */

struct UnaryOpNode : ExprNode {
	inst op;
	Node *arg;

	UnaryOpNode(inst opIn, Node *argIn) : ExprNode(kUnaryOpNode), op(opIn), arg(argIn) {}
	virtual ~UnaryOpNode() {
		delete arg;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitUnaryOpNode(this);
	}
};

/* BinaryOpNode */

struct BinaryOpNode : ExprNode {
	inst op;
	Node *a;
	Node *b;

	BinaryOpNode(inst opIn, Node *aIn, Node *bIn) : ExprNode(kBinaryOpNode), op(opIn), a(aIn), b(bIn) {}
	virtual ~BinaryOpNode() {
		delete a;
		delete b;
	}
	virtual void accept(NodeVisitor *visitor) {
		visitor->visitBinaryOpNode(this);
	}
};

} // End of namespace Director

#endif