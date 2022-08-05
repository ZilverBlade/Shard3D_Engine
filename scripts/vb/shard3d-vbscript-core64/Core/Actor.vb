Imports System
Imports Shard3D.Components

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
            Return InternalCalls.Actor_HasComponent(ID, componentType)
        End Function

        Public Function GetComponent(Of T As {Component, New})() As T
            If Not HasComponent(Of T)() Then Return Nothing
            Dim component As T = New T() With {
                ._Actor = Me
            }
            Return component
        End Function
    End Class
End Namespace
