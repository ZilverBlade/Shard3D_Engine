Imports System

Namespace Shard3D.Core
    Public Class Actor
        Protected Sub New()
            ID = 1
        End Sub

        Friend Sub New(ByVal _id As ULong)
            ID = _id
        End Sub

        Public ReadOnly ID As ULong

        Public Function HasComponent(Of T As {Component, New})() As Boolean
            Dim componentType As Type = GetType(T)
            Return InternalCalls.Actor_HasComponent(ID, componentType, 1)
        End Function

        Public Function GetComponent(Of T As {Component, New})() As T
            If Not HasComponent(Of T)() Then
                InternalCalls.Log("Tried to get component that does not exist!", LogSeverity.[Error])
                Return Nothing
            End If

            Dim component As T = New T() With {
                ._Actor = Me
            }
            Return component
        End Function

        Public Function AddComponent(Of T As {Component, New})() As T
            If HasComponent(Of T)() Then
                InternalCalls.Log("Tried to add existing component!", LogSeverity.Warn)
                Return Nothing
            End If

            Dim componentType As Type = GetType(T)
            InternalCalls.Actor_AddComponent(ID, componentType, 1)
            Dim component As T = New T() With {
                ._Actor = Me
            }
            Return component
        End Function

        Public Sub KillComponent(Of T As {Component, New})()
            If Not HasComponent(Of T)() Then
                InternalCalls.Log("Tried to remove component that does not exist!", LogSeverity.Warn)
                Return
            End If

            Dim componentType As Type = GetType(T)
            InternalCalls.Actor_RmvComponent(ID, componentType, 1)
        End Sub
    End Class
End Namespace
