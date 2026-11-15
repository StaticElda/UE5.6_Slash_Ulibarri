#pragma once


#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 100.f, 12, FColor::Cyan, true );
#define DRAW_LINE(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Cyan, true, -1.f, 0, 1.f );
#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 20, FColor::Cyan, true);
#define DRAW_VECTOR(StartLocation,EndLocation) if(GetWorld())\
{\
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Cyan, true, -1.f, 0, 1.f );\
    DrawDebugPoint(GetWorld(), EndLocation, 20, FColor::Cyan, true);\
}

#define DRAW_SPHERE_SINGLEFRAME(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 100.f, 12, FColor::Cyan, false, -1.f );
#define DRAW_LINE_SINGLEFRAME(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Cyan, false, -1.f, 0, 1.f );
#define DRAW_POINT_SINGLEFRAME(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 20, FColor::Cyan, false, -1.f);
#define DRAW_VECTOR_SINGLEFRAME(StartLocation,EndLocation) if(GetWorld())\
{\
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Cyan, false, -1.f, 0, 1.f );\
    DrawDebugPoint(GetWorld(), EndLocation, 20, FColor::Cyan, false, -1.f);\
}