// Testcase.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "assert.h"
#include "../JsonPacketLib/JsonNode.h"
#include "../JsonPacketLib/JsonBuilder.h"

class CTestFunctionPrint
{
public:
	CTestFunctionPrint(const char* functionname) : m_functionname(functionname)
	{
		printf("Begin %s\n", m_functionname);
	}

	~CTestFunctionPrint()
	{
		printf("Finsh %s\n", m_functionname);
	}

private:
	const char* m_functionname;
};

void TestNode()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);

	tstring TestName = "alice";
	JsonObjectNode node;
	node.Add("StrValue", TestName);
	tstring name0 = node.GetValue<tstring>("StrValue");
	assert(TestName == name0);

	int TestInteger = 23498;
	node.Add("Integer", TestInteger);
	int number0 = node.GetValue<int>("Integer");
	assert(number0 == TestInteger);
	tstring name1 = node.GetValue<tstring>("StrValue");
	assert(TestName == name1);

	int TestNeInteger = -23498;
	node.Add("SignedInteger", TestNeInteger);
	int number1 = node.GetValue<int>("SignedInteger");
	assert(number1 == TestNeInteger);

	unsigned int TestUinteger = 4294967295;
	node.Add("UInteger", TestUinteger);
	unsigned int uInt0 = node.GetValue<unsigned int>("UInteger");
	assert(uInt0 == TestUinteger);

	float TestFloat = 2113432.5646f;
	node.Add("FloatValue", TestFloat);
	float floatResult = node.GetValue<float>("FloatValue");
	assert(floatResult == TestFloat);

	tstring content = node.ToString();
	JsonObjectNode node1;
	node1.Parse(content);
	unsigned int uInt1 = node1.GetValue<unsigned int>("UInteger");
	assert(uInt1 == TestUinteger);
}

void TestArrayNode()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);
	const int ElementCount = 2;
	tstring TestName[ElementCount] = {"alice", "knight"};
	int TestLevel[ElementCount] = {22, 43};
	float TestExp[ElementCount] = {232254.4456f, 7346565.443f};

	JsonArrayNode arrayNode0;
	for (int i=0; i<ElementCount; ++i)
	{
		JsonObjectNode objectNode;
		objectNode.Add("name", TestName[i]);
		objectNode.Add("level", TestLevel[i]);
		objectNode.Add("exp", TestExp[i]);

		arrayNode0.Insert(objectNode);
	}
	assert(ElementCount == arrayNode0.Size());
	for (int i=0; i<ElementCount; ++i)
	{
		JsonObjectNode objectNode = arrayNode0.GetObjectNode(i);
		tstring name = objectNode.GetValue<tstring>("name");
		int level = objectNode.GetValue<int>("level");
		float exp = objectNode.GetValue<float>("exp");
		assert(name == TestName[i]);
		assert(level == TestLevel[i]);
		assert(exp == TestExp[i]);
	}
	tstring content = arrayNode0.ToString();
	JsonArrayNode arrayNode1;
	arrayNode1.Parse(content);

	assert(arrayNode0 == arrayNode1);
	assert(arrayNode0.ToString() == arrayNode1.ToString());
}

void TestOperator()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);
	JsonObjectNode srcObjNode, dstObjNode;
	srcObjNode.Add("name", "alice");
	dstObjNode = srcObjNode;
	assert(srcObjNode == dstObjNode);
	assert(srcObjNode.ToString() == dstObjNode.ToString());

	JsonArrayNode srcArrayNode, dstArrayNode;
	srcArrayNode.Insert(srcObjNode);
	dstArrayNode = srcArrayNode;
	assert(srcArrayNode == dstArrayNode);
	assert(srcArrayNode.ToString() == dstArrayNode.ToString());
}

void TestChildNode()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);
	tstring TestName = "alice";
	JsonObjectNode childNode0;
	childNode0.Add("name", TestName);
	int TestLevel = 57;
	childNode0.Add("level", TestLevel);

	JsonObjectNode parentNode;
	parentNode.Add("npc", &childNode0);
	JsonObjectNode chileNode1 = parentNode.GetValue<JsonObjectNode>("npc");
	tstring name = chileNode1.GetValue<tstring>("name");
	assert(name == TestName);
	int level = chileNode1.GetValue<int>("level");
	assert(level == TestLevel);
	assert(childNode0 == chileNode1);
	assert(childNode0.ToString() == chileNode1.ToString());
}

void TestMakeBuffer()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);
	tstring TestName = "alice";
	JsonObjectNode srcNode;
	srcNode.Add("StrValue", TestName);
	int TestInteger = 23498;
	srcNode.Add("Integer", TestInteger);
	int TestNeInteger = -23498;
	srcNode.Add("SignedInteger", TestNeInteger);
	unsigned int TestUinteger = 4294967295;
	srcNode.Add("UInteger", TestUinteger);
	float TestFloat = 2113432.5646f;
	srcNode.Add("FloatValue", TestFloat);

	const int BufferSize = 1024;
	char buffer[BufferSize] = {0,};
	unsigned int writtenSize = JsonBuilder::MakeBuffer(srcNode, buffer, BufferSize);

	JsonObjectNode dstNode;
	dstNode.Parse(buffer, writtenSize);
	assert(srcNode == dstNode);
}

void TestMakeBufferWithArray()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);

	const int ElementCount = 3;
	tstring ItemName[ElementCount] = {"onehanded-axe", "short-sword", "spear"};
	int ItemPrice[ElementCount] = {5, 8, 21};
	float ItemPower[ElementCount] = {8.12f, 5.9f, 12.0f};

	JsonArrayNode srcItemArray;
	for (int i=0; i<ElementCount; ++i)
	{
		JsonObjectNode itemNode;
		itemNode.Add("name", ItemName[i]);
		itemNode.Add("price", ItemPrice[i]);
		itemNode.Add("power", ItemPower[i]);
		srcItemArray.Insert(itemNode);
	}

	JsonObjectNode srcRootNode;
	srcRootNode.Add("items", &srcItemArray);
	srcRootNode.Add("race", "human");
	
	const int BufferSize = 1024;
	char buffer[BufferSize] = {0,};
	unsigned int writtenSize = JsonBuilder::MakeBuffer(srcRootNode, buffer, BufferSize);

	JsonBuilder builder;
	builder.PushBuffer(buffer, writtenSize);
	JsonObjectNode dstRootNode;
	assert(builder.PopCompleteNode(dstRootNode));
	assert(srcRootNode == dstRootNode);

	tstring human = dstRootNode.GetValue<tstring>("race");
	assert(human == "human");

	JsonArrayNode dstItemArray = dstRootNode.GetValue<JsonArrayNode>("items");
	assert(srcItemArray == dstItemArray);
	assert(srcItemArray.ToString() == dstItemArray.ToString());
	for (int i=0; i<ElementCount; ++i)
	{
		JsonObjectNode dstObj = dstItemArray.GetObjectNode(i);
		JsonObjectNode srcObj = srcItemArray.GetObjectNode(i);
		assert(dstObj == srcObj);
		assert(ItemName[i] == dstObj.GetValue<tstring>("name"));
		assert(ItemPrice[i] == dstObj.GetValue<int>("price"));
		assert(ItemPower[i] == dstObj.GetValue<float>("power"));
	}
}

void TestSeperatePacket()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);

	tstring TestName = "alice";
	JsonObjectNode srcNode;
	srcNode.Add("StrValue", TestName);
	int TestInteger = 23498;
	srcNode.Add("Integer", TestInteger);
	int TestNeInteger = -23498;
	srcNode.Add("SignedInteger", TestNeInteger);
	unsigned int TestUinteger = 4294967295;
	srcNode.Add("UInteger", TestUinteger);
	float TestFloat = 2113432.5646f;
	srcNode.Add("FloatValue", TestFloat);

	const int BufferSize = 1024;
	char buffer[BufferSize] = {0,};
	unsigned int writtenSize = JsonBuilder::MakeBuffer(srcNode, buffer, BufferSize);
	unsigned int inCompleteSize = writtenSize - 5;
	JsonBuilder builder;
	builder.PushBuffer(buffer, inCompleteSize);

	JsonObjectNode dstNode;
	bool result = builder.PopCompleteNode(dstNode);
	assert(result == false);
	
	unsigned int leftSize = writtenSize-inCompleteSize;
	builder.PushBuffer(&buffer[inCompleteSize], leftSize);
	result = builder.PopCompleteNode(dstNode);
	assert(result == true);

	assert(srcNode == dstNode);
}

void TestGluePacket()
{
	CTestFunctionPrint autoPrint(__FUNCTION__);

	tstring TestName = "alice";
	JsonObjectNode srcNode0;
	srcNode0.Add("name", TestName);
	int TestLevel = 234;
	srcNode0.Add("level", TestLevel);

	const int BufferSize = 1024;
	char buffer[BufferSize] = {0,};
	unsigned int writtenSize = JsonBuilder::MakeBuffer(srcNode0, buffer, BufferSize);
	
	JsonBuilder builder;
	builder.PushBuffer(buffer, writtenSize);

	tstring MonsterName = "queen";
	JsonObjectNode srcNode1;
	srcNode1.Add("name", MonsterName);
	float MonsterExp = 234324.23f;
	srcNode1.Add("exp", MonsterExp);
	writtenSize = JsonBuilder::MakeBuffer(srcNode1, buffer, BufferSize);
	builder.PushBuffer(buffer, writtenSize);

	JsonObjectNode dstNode0, dstNode1;
	bool result = builder.PopCompleteNode(dstNode0);
	assert(result == true);
	assert(srcNode0 == dstNode0);
	result = builder.PopCompleteNode(dstNode1);
	assert(result == true);
	assert(srcNode1 == dstNode1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	TestNode();
	TestArrayNode();
	TestOperator();
	TestChildNode();
	TestMakeBuffer();
	TestMakeBufferWithArray();
	TestSeperatePacket();
	TestGluePacket();
	printf("\nTest success.");
	getchar();
	return 0;
}

