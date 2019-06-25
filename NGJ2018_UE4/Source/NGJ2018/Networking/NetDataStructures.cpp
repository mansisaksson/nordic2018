#include "NetDataStructures.h"
#include "Serialization/JsonSerializer.h"

namespace _NetDataSerializer
{
	const FString GravDirAttr = TEXT("gravityDirection");
	const FString PacketIdAttr = TEXT("packetId");
	const FString ObjArrayAttr = TEXT("jsonPackages");

	const FString posXAttr = TEXT("positionX");
	const FString posYAttr = TEXT("positionY");
	const FString posZAttr = TEXT("positionZ");

	const FString AngleAttr = TEXT("angle");

	const FString scaleXAttr = TEXT("scaleX");
	const FString scaleYAttr = TEXT("scaleY"); // Flip y and Z
	const FString scaleZAttr = TEXT("scaleZ");
	const FString TypeAttr = TEXT("type");
	const FString IdAttr = TEXT("id");
}

TSharedPtr<FJsonObject> FNGObject::ToJsonObject()
{
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	jsonObject->SetNumberField(_NetDataSerializer::posXAttr, position.X);
	jsonObject->SetNumberField(_NetDataSerializer::posYAttr, position.Y);
	jsonObject->SetNumberField(_NetDataSerializer::posZAttr, position.Z);

	jsonObject->SetNumberField(_NetDataSerializer::AngleAttr, rotationAngle);

	jsonObject->SetNumberField(_NetDataSerializer::scaleXAttr, scale.X);
	jsonObject->SetNumberField(_NetDataSerializer::scaleYAttr, scale.Y);
	jsonObject->SetNumberField(_NetDataSerializer::scaleZAttr, scale.Z);

	jsonObject->SetNumberField(_NetDataSerializer::IdAttr, objecId);

	jsonObject->SetStringField(_NetDataSerializer::TypeAttr, type);

	return jsonObject;
}

FNGObject FNGObject::FromJsonObject(const TSharedPtr<FJsonObject> &jsonObject)
{
	FNGObject retObj;
	if (jsonObject.IsValid())
	{
		retObj.position.X = jsonObject->GetNumberField(_NetDataSerializer::posZAttr) * -100.f;
		retObj.position.Y = jsonObject->GetNumberField(_NetDataSerializer::posXAttr) * 100.f;
		retObj.position.Z = jsonObject->GetNumberField(_NetDataSerializer::posYAttr) * 100.f;

		retObj.rotationAngle = jsonObject->GetNumberField(_NetDataSerializer::AngleAttr);

		retObj.scale.X = jsonObject->GetNumberField(_NetDataSerializer::scaleZAttr);
		retObj.scale.Y = jsonObject->GetNumberField(_NetDataSerializer::scaleXAttr); 
		retObj.scale.Z = jsonObject->GetNumberField(_NetDataSerializer::scaleYAttr);

		retObj.objecId = jsonObject->GetNumberField(_NetDataSerializer::IdAttr);
		retObj.type = jsonObject->GetStringField(_NetDataSerializer::TypeAttr);
	}

	return retObj;
}

FString FNGPacket::Serialize()
{
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject);

	jsonObject->SetNumberField(_NetDataSerializer::GravDirAttr, gravityDir);
	jsonObject->SetNumberField(_NetDataSerializer::PacketIdAttr, packetID);

	TArray<TSharedPtr<FJsonValue>> jsonObjectArray;
	for (FNGObject &ngObject : objects)
		jsonObjectArray.Add(MakeShareable(new FJsonValueObject(ngObject.ToJsonObject())));
		
		
	jsonObject->SetArrayField(_NetDataSerializer::ObjArrayAttr, jsonObjectArray);

	FString outputString;
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&outputString));
	//UE_LOG(LogTemp, Log, TEXT("Serialize: %s"), *outputString);
	return outputString;
}

FNGPacket FNGPacket::Deserialize(const FString &jsonString)
{
	FNGPacket outPacket;
	//UE_LOG(LogTemp, Log, TEXT("Deserialize: %s"), *jsonString);
	TSharedPtr<FJsonObject> jsonObject;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(jsonString), jsonObject))
	{
		outPacket.gravityDir = jsonObject->GetNumberField(_NetDataSerializer::GravDirAttr);

		outPacket.packetID = jsonObject->GetNumberField(_NetDataSerializer::PacketIdAttr);

		const TArray<TSharedPtr<FJsonValue>>& objJsonAray = jsonObject->GetArrayField(_NetDataSerializer::ObjArrayAttr);
		for (const TSharedPtr<FJsonValue> &jsonValueRef : objJsonAray)
			if (jsonValueRef.IsValid())
				outPacket.objects.Add(FNGObject::FromJsonObject(jsonValueRef->AsObject()));
	}

	return outPacket;
}