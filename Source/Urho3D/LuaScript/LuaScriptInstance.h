//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../LuaScript/LuaScriptEventListener.h"
#include "../Scene/Component.h"
#include <kaguya.hpp>

struct lua_State;

namespace Urho3D
{

class LuaFile;
class LuaFunction;
class LuaScript;
class LuaScriptEventInvoker;

/// Lua Script object methods.
enum LuaScriptObjectMethod
{
    LSOM_START = 0,
    LSOM_STOP,
    LSOM_DELAYEDSTART,
    LSOM_UPDATE,
    LSOM_POSTUPDATE,
    LSOM_FIXEDUPDATE,
    LSOM_FIXEDPOSTUPDATE,
    LSOM_LOAD,
    LSOM_SAVE,
    LSOM_READNETWORKUPDATE,
    LSOM_WRITENETWORKUPDATE,
    LSOM_APPLYATTRIBUTES,
    LSOM_TRANSFORMCHANGED,
    MAX_LUA_SCRIPT_OBJECT_METHODS
};

/// Lua script object component.
class URHO3D_API LuaScriptInstance : public Component, public LuaScriptEventListener
{
    URHO3D_OBJECT(LuaScriptInstance, Component);

public:
    /// Construct.
    LuaScriptInstance(Context* context);
    /// Destruct.
    ~LuaScriptInstance();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Handle attribute write access.
    virtual void OnSetAttribute(const AttributeInfo& attr, const Variant& src);
    /// Handle attribute read access.
    virtual void OnGetAttribute(const AttributeInfo& attr, Variant& dest) const;

    /// Return attribute descriptions, or null if none defined.
    virtual const Vector<AttributeInfo>* GetAttributes() const { return &attributeInfos_; }

    /// Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    virtual void ApplyAttributes();
    /// Handle enabled/disabled state change.
    virtual void OnSetEnabled();

    /// Add a scripted event handler by function.
    virtual void AddEventHandler(StringHash eventType, int functionIndex);
	/// Add a scripted event handler by function name.
    virtual void AddEventHandler(StringHash eventType, const String& functionName);
    /// Add a scripted event handler by function for a specific sender.
    virtual void AddEventHandler(Object* sender, StringHash eventType, int functionIndex);
    /// Add a scripted event handler by function name for a specific sender.
    virtual void AddEventHandler(Object* sender, StringHash eventType, const String& functionName);
    /// Remove a scripted event handler.
    virtual void RemoveEventHandler(StringHash eventType);
    /// Remove a scripted event handler for a specific sender.
    virtual void RemoveEventHandler(Object* sender, StringHash eventType);
    /// Remove all scripted event handlers for a specific sender.
    virtual void RemoveEventHandlers(Object* sender);
    /// Remove all scripted event handlers.
    virtual void RemoveAllEventHandlers();
    /// Remove all scripted event handlers, except those listed.
    virtual void RemoveEventHandlersExcept(const PODVector<StringHash>& exceptionTypes);
    /// Return whether has subscribed to an event.
    virtual bool HasEventHandler(StringHash eventType) const;
    /// Return whether has subscribed to a specific sender's event.
    virtual bool HasEventHandler(Object* sender, StringHash eventType) const;

    /// Create script object. Return true if successful.
    bool CreateObject(const String& scriptObjectType);
    /// Create script object. Return true if successful.
    bool CreateObject(LuaFile* scriptFile, const String& scriptObjectType);
    /// Set script file.
    void SetScriptFile(LuaFile* scriptFile);
    /// Set script object type.
    void SetScriptObjectType(const String& scriptObjectType);
    /// Set script file serialization attribute by calling a script function.
    void SetScriptDataAttr(const PODVector<unsigned char>& data);
    /// Set script network serialization attribute by calling a script function.
    void SetScriptNetworkDataAttr(const PODVector<unsigned char>& data);

    /// Return script file.
    LuaFile* GetScriptFile() const;
    /// Return script object type.
    const String& GetScriptObjectType() const { return scriptObjectType_; }
    /// Return Lua reference to script object.
    int GetScriptObjectRef() const { return scriptObjectRef_; }

    /// Get script file serialization attribute by calling a script function.
    PODVector<unsigned char> GetScriptDataAttr() const;
    /// Get script network serialization attribute by calling a script function.
    PODVector<unsigned char> GetScriptNetworkDataAttr() const;
    /// Return lua state.
    lua_State* GetLuaState() const { return luaState_; }
    /// Return script object's funcition.
    LuaFunction* GetScriptObjectFunction(const String& functionName) const;
    /// Set script file attribute.
    void SetScriptFileAttr(const ResourceRef& value);
    /// Return script file attribute.
    ResourceRef GetScriptFileAttr() const;

protected:
    /// Handle scene being assigned.
    virtual void OnSceneSet(Scene* scene);
    /// Handle node transform being dirtied.
    virtual void OnMarkedDirty(Node* node);

private:
    /// Find script object attributes.
    void GetScriptAttributes();
    /// Find script object method refs.
    void FindScriptObjectMethodRefs();
    /// Subscribe to script method events.
    void SubscribeToScriptMethodEvents();
    /// Unsubscribe from script method events.
    void UnsubscribeFromScriptMethodEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the logic post update event.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
#if defined(URHO3D_PHYSICS) || defined(URHO3D_URHO2D)
    /// Handle the physics update event.
    void HandleFixedUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle the physics post update event.
    void HandlePostFixedUpdate(StringHash eventType, VariantMap& eventData);
#endif
    /// Release the script object.
    void ReleaseObject();

    // Lua Script subsystem.
    LuaScript* luaScript_;
    /// Lua state.
    lua_State* luaState_;
    /// Event invoker.
    SharedPtr<LuaScriptEventInvoker> eventInvoker_;
    /// Script file.
    SharedPtr<LuaFile> scriptFile_;
    /// Script object type.
    String scriptObjectType_;
    /// Attributes, including script object variables.
    Vector<AttributeInfo> attributeInfos_;
    /// Lua reference to script object.
    int scriptObjectRef_;

    /// Script object method.
    LuaFunction* scriptObjectMethods_[MAX_LUA_SCRIPT_OBJECT_METHODS];
};

}
