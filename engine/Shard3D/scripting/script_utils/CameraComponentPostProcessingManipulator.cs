using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shard3D.Core
{
   //CameraComponentPostProcessingManipulator
    public static class CCPPM
    {
        public static uint GetPostProcessingEffectsCount(Actor actor)
        {
            InternalCalls.CameraComponent_GetPostProcessingEffectsCount(actor.ID, out uint _val);
            return _val;
        }
        public static uint GetPostProcessingParametersCount(Actor actor, uint _index)
        {
            InternalCalls.CameraComponent_GetPostProcessingEffectsCount(actor.ID, out uint _val);
            return _val;
        }
        public static int GetPostProcessingParameterValue_int(Actor actor, uint _effect_index, uint _param_index)
        {
            InternalCalls.CameraComponent_GetPostProcessingParameterValue_int(actor.ID, _effect_index, _param_index, out int _val); 
            return _val;
        }
        public static void SetPostProcessingParameterValue_int(Actor actor, uint _effect_index, uint _param_index, int value)
        {
            InternalCalls.CameraComponent_SetPostProcessingParameterValue_int(actor.ID, _effect_index, _param_index, ref value);
        }
        public static float GetPostProcessingParameterValue_float(Actor actor, uint _effect_index, uint _param_index)
        {
            InternalCalls.CameraComponent_GetPostProcessingParameterValue_float(actor.ID, _effect_index, _param_index, out float _val); 
            return _val;
        }
        public static void SetPostProcessingParameterValue_float(Actor actor, uint _effect_index, uint _param_index, float value)
        {
            InternalCalls.CameraComponent_SetPostProcessingParameterValue_float(actor.ID, _effect_index, _param_index, ref value);
        }
        public static Vector2 GetPostProcessingParameterValue_float2(Actor actor, uint _effect_index, uint _param_index)
        {
            InternalCalls.CameraComponent_GetPostProcessingParameterValue_float2(actor.ID, _effect_index, _param_index, out Vector2 _val);
            return _val;
        }
        public static void SetPostProcessingParameterValue_float2(Actor actor, uint _effect_index, uint _param_index, Vector2 value)
        {
            InternalCalls.CameraComponent_SetPostProcessingParameterValue_float2(actor.ID, _effect_index, _param_index, ref value);
        }
        public static Vector4 GetPostProcessingParameterValue_float4(Actor actor, uint _effect_index, uint _param_index)
        {
            InternalCalls.CameraComponent_GetPostProcessingParameterValue_float4(actor.ID, _effect_index, _param_index, out Vector4 _val);
            return _val;
        }
        public static void SetPostProcessingParameterValue_float4(Actor actor, uint _effect_index, uint _param_index, Vector4 value)
        {
            InternalCalls.CameraComponent_SetPostProcessingParameterValue_float4(actor.ID, _effect_index, _param_index, ref value);
        }
        public static System.Type GetPostProcessingParameterValueType(Actor actor, uint _effect_index, uint _param_index)
        {
            InternalCalls.CameraComponent_GetPostProcessingParameterValueType(actor.ID, _effect_index, _param_index, out System.Type _val);
            return _val;
        }
    }
}
