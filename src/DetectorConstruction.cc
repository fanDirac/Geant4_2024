#include "../include/DetectorConstruction.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4Sphere.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4LogicalBorderSurface.hh"
#include "../include/SensitiveDetector.hh"

DetectorConstruction::DetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

// В качестве детектора антинейтрино от ядерного реактора используется сцинтиллятор
// на основе линейного алкилбензола (ЛАБ) с сцинтилляционной добавкой PPO (3 г/л) и солью
// гадолиния с концентрацией гадолиния в сцинтилляторе 0.5 грамм/литр. Антинейтрино
// регистрируются в реакции обратного бета распада антинейтрино на протоне (водороде в
// составе ЛАБ). Сцинтиллятор объемом 1 м3 размещается в сферическом сосуде из
// полиметилметакрилата (ПММА) с толшиной стенок 10 мм. Сфера со сцинтиллятором
// расположена в центре цилиндрического бака из нержавеющей стали с внутренними
// размерами: диаметр=высоте=1858 мм (толщина стали 2 мм), заполненного линейным
// алкилбензолом (ЛАБ) без сцинтилляционных добавок. Внутри бака по вершинам
// правильного многоугольника (додекаэдра либо икосаэдра) расположены фотоэлектронные
// умножители (ФЭУ) марки Hamamatsu R5912. Световыход сцинтиллятора — 5000
// фотонов/МэВ, прозрачность сцинтиллятора 5 метров, прозрачность ЛАБ 12 метров,
// квантовая эффективность ФЭУ 28%.

G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Инициализация менеджера материалов 
    G4NistManager* nist = G4NistManager::Instance();
    
    // Материалы
    G4Element* C = nist->FindOrBuildElement("C"); 
    G4Element* H = nist->FindOrBuildElement("H");

    G4Material* LAB = new G4Material("LAB", 0.856 * g/cm3, 2);
    LAB->AddElement(C, 10);
    LAB->AddElement(H, 22);

    G4Element* O = nist->FindOrBuildElement("O");
    G4Element* N = nist->FindOrBuildElement("N");

    G4Material* PPO = new G4Material("PPO", 1.094 * g / cm3, 4);  
    PPO->AddElement(C, 15);  
    PPO->AddElement(H, 11);  
    PPO->AddElement(O, 1);  
    PPO->AddElement(N, 1);   
    
    G4Element* Gd = nist->FindOrBuildElement("Gd");
    G4Material* GdMaterial = new G4Material("Gd", 4.139 * g / cm3, 1);
    GdMaterial->AddElement(Gd, 1);

    // Материал сцинтиллятора 
    G4Material* Scintillator = new G4Material("Scintillator", 0.87 * g / cm3, 3);
    G4double ppoFr = 3.0 / (856.0 + 3.0 + 0.5);
    G4double gdFr = 0.5 / (856.0 + 3.0 + 0.5);
    Scintillator->AddMaterial(LAB, 1 - ppoFr - gdFr);
    Scintillator->AddMaterial(PPO, ppoFr);  
    Scintillator->AddMaterial(GdMaterial, gdFr);

    G4Material* PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    G4Material* steel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

    // Оптические свойства
    G4double energy[] = {1.7 * eV, 2.5 * eV, 3.4 * eV};  

    G4double rindexLAB[] = {1.51, 1.52, 1.53}; 
    G4double absorptionLAB[] = {12.0 * m, 12.0 * m, 12.0 * m}; 

    G4MaterialPropertiesTable* LABMPT = new G4MaterialPropertiesTable();
    LABMPT->AddProperty("RINDEX", energy, rindexLAB, 3);
    LABMPT->AddProperty("ABSLENGTH", energy, absorptionLAB, 3);
    LAB->SetMaterialPropertiesTable(LABMPT);

    G4double scintScintillator[] = {0.1, 0.8, 0.1};
    G4double rindexScintillator[] = {1.51, 1.52, 1.53};
    G4double absorptionScintillator[] = {5.0 * m, 5.0 * m, 5.0 * m};

    G4MaterialPropertiesTable* scintillatorMPT = new G4MaterialPropertiesTable();
    scintillatorMPT->AddProperty("SCINTILLATIONCOMPONENT1", energy, scintScintillator, 3);
    scintillatorMPT->AddProperty("RINDEX", energy, rindexScintillator, 3);
    scintillatorMPT->AddProperty("ABSLENGTH", energy, absorptionScintillator, 3);
    scintillatorMPT->AddConstProperty("SCINTILLATIONYIELD", 5000.0 * 0.28 / MeV);
    scintillatorMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    scintillatorMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 0.0 * ns);
    Scintillator->SetMaterialPropertiesTable(scintillatorMPT);

    G4double rindexPMMA[] = {1.49, 1.49, 1.49};  
    G4double absorptionPMMA[] = {10.0 * m, 10.0 * m, 10.0 * m};  

    G4MaterialPropertiesTable* PMMAMPT = new G4MaterialPropertiesTable();
    PMMAMPT->AddProperty("RINDEX", energy, rindexPMMA, 3);
    PMMAMPT->AddProperty("ABSLENGTH", energy, absorptionPMMA, 3);
    PMMA->SetMaterialPropertiesTable(PMMAMPT);

    //  Мир 
    G4Box* solidWorld = new G4Box("solidWorld", 2 * m, 2 * m, 2 * m);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, nist->FindOrBuildMaterial("G4_AIR"), "logicWorld");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

    //Цилиндр из стали 
    G4double tankHeight = 1858 * mm;
    G4double tankRadius = tankHeight/2; 
    G4Tubs* solidTank = new G4Tubs("solidTank", 0, tankRadius, tankHeight / 2, 0, 360 * deg);
    G4LogicalVolume* logicTank = new G4LogicalVolume(solidTank, steel, "logicTank");
    new G4PVPlacement(0, G4ThreeVector(), logicTank, "physTank", logicWorld, false, 0, true);

    //Внутренний бак с ЛАБ 
    G4double tankInnerHeight = tankHeight - 2 * mm;
    G4double tankInnerRadius = tankRadius - 2 * mm;
    G4Tubs* solidInnerTank = new G4Tubs("solidInnerTank", 0, tankInnerRadius, tankInnerHeight / 2, 0, 360 * deg);
    G4LogicalVolume* logicInnerTank = new G4LogicalVolume(solidInnerTank, LAB, "logicInnerTank");
    G4VPhysicalVolume* physInnerTank = new G4PVPlacement(0, G4ThreeVector(), logicInnerTank, "physInnerTank", logicTank, false, 0, true);
    
    //Сфера из ПММА
    G4double sphereRadius = 0.62035 * m + 10 * mm; 
    G4Sphere* solidSphere = new G4Sphere("solidSphere", 0, sphereRadius, 0, 360 * deg, 0, 180 * deg);
    G4LogicalVolume* logicSphere = new G4LogicalVolume(solidSphere, PMMA, "logicSphere");
    G4VPhysicalVolume* physSphere = new G4PVPlacement(0, G4ThreeVector(), logicSphere, "physSphere", logicInnerTank, false, 0, true);

    //Внутренняя сфера - сцинтиллятор
    G4double scintillatorRadius = sphereRadius - 10 * mm; 
    G4Sphere* solidScintillator = new G4Sphere("solidScintillator", 0, scintillatorRadius, 0, 360 * deg, 0, 180 * deg);
    G4LogicalVolume* logicScintillator = new G4LogicalVolume(solidScintillator, Scintillator    , "logicScintillator");
    G4VPhysicalVolume* physScintillator = new G4PVPlacement(0, G4ThreeVector(), logicScintillator, "physScintillator", logicSphere, false, 0, true);

    G4OpticalSurface* scintillatorPMMASurface = new G4OpticalSurface("ScintillatorPMMASurface");
    scintillatorPMMASurface->SetType(dielectric_dielectric);
    scintillatorPMMASurface->SetFinish(polished);
    scintillatorPMMASurface->SetModel(unified);

    G4MaterialPropertiesTable* scintillatorPMMAMPT = new G4MaterialPropertiesTable();
    G4double reflectivity[] = {0.8, 0.8, 0.8}; 
    scintillatorPMMAMPT->AddProperty("REFLECTIVITY", energy, reflectivity, 3);
    scintillatorPMMASurface->SetMaterialPropertiesTable(scintillatorPMMAMPT);

    new G4LogicalBorderSurface("ScintillatorPMMASurface", physScintillator, physSphere, scintillatorPMMASurface);

    // ФЭУ 
    G4double phMulRadius = 131 * mm;                        
    G4Sphere* phMul = new G4Sphere("solidSector", 0, phMulRadius, 0, 360 * deg, 0, 93 / 2 * deg);
    G4LogicalVolume* logicPhMul = new G4LogicalVolume(phMul, LAB, "logicPhMul");

    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    SensitiveDetector* sensDet = new SensitiveDetector("SensitiveDetector");
    SDman->AddNewDetector(sensDet);
    logicPhMul->SetSensitiveDetector(sensDet);

    G4double dodecahedrRadius = 0.8 * m ; 
    // std::vector<G4ThreeVector> dodecahedrVertices = {
    //     G4ThreeVector(1, 1, 1),G4ThreeVector(1, 1, -1),G4ThreeVector(1, -1, 1),G4ThreeVector(1, -1, -1),
    //     G4ThreeVector(-1, 1, 1),G4ThreeVector(-1, 1, -1),G4ThreeVector(-1, -1, 1),G4ThreeVector(-1, -1, -1),
    //     G4ThreeVector(0, 1 / 1.618, 1.618),G4ThreeVector(0, 1 / 1.618, -1.618),G4ThreeVector(0, -1 / 1.618, 1.618),
    //     G4ThreeVector(0, -1 / 1.618, -1.618),G4ThreeVector(1 / 1.618, 1.618, 0),G4ThreeVector(1 / 1.618, -1.618, 0),
    //     G4ThreeVector(-1 / 1.618, 1.618, 0),G4ThreeVector(-1 / 1.618, -1.618, 0),G4ThreeVector(1.618, 0, 1 / 1.618),
    //     G4ThreeVector(1.618, 0, -1 / 1.618),G4ThreeVector(-1.618, 0, 1 / 1.618),G4ThreeVector(-1.618, 0, -1 / 1.618)
    // };
    
    // for (size_t i = 0; i < dodecahedrVertices.size(); ++i)
    //     dodecahedrVertices[i] = dodecahedrVertices[i].unit() * dodecahedrRadius;
    
    // for (size_t i = 0; i < dodecahedrVertices.size(); ++i) {
    //     G4ThreeVector directionToCenter = -dodecahedrVertices[i];
    //     G4RotationMatrix* rotation = new G4RotationMatrix();

    //     if(directionToCenter.z() > 0){
    //         rotation->rotateX(atan2(directionToCenter.y(), directionToCenter.z()));
    //         rotation->rotateY(-atan2(directionToCenter.x(), directionToCenter.z()));
    //     } else if (directionToCenter.z() < 0){
    //         rotation->rotateX(atan2(directionToCenter.y(), directionToCenter.z()));
    //         rotation->rotateY(atan2(directionToCenter.x(), directionToCenter.z()));
    //         rotation->rotateX(180 * deg);
    //     } else{
    //         rotation->rotateX((directionToCenter.y() >= 0) ? 90 * deg : -90 * deg);
    //         if (directionToCenter.y() > 0) rotation->rotateY(-atan2(directionToCenter.x(), directionToCenter.y()));
    //         else{rotation->rotateY(atan2(directionToCenter.x(), directionToCenter.y()));
    //             rotation->rotateY(180 * deg);
    //         }
    //     }

    //     new G4PVPlacement(rotation, dodecahedrVertices[i], logicPhMul, "physPhMul", logicInnerTank, false, i, true);
    // }
    std::vector<G4ThreeVector> icosahedronVertices = {
        G4ThreeVector(0, 1, 1.618),    G4ThreeVector(0, -1, 1.618),
        G4ThreeVector(0, 1, -1.618),   G4ThreeVector(0, -1, -1.618),
        G4ThreeVector(1, 1.618, 0),    G4ThreeVector(-1, 1.618, 0),
        G4ThreeVector(1, -1.618, 0),   G4ThreeVector(-1, -1.618, 0),
        G4ThreeVector(1.618, 0, 1),    G4ThreeVector(-1.618, 0, 1),
        G4ThreeVector(1.618, 0, -1),   G4ThreeVector(-1.618, 0, -1)
    };
    for (size_t i = 0; i < icosahedronVertices.size(); ++i)
        icosahedronVertices[i] = icosahedronVertices[i].unit() * dodecahedrRadius;
    
    for (size_t i = 0; i < icosahedronVertices.size(); ++i) {
        G4ThreeVector directionToCenter_ico = -icosahedronVertices[i];
        G4RotationMatrix* rotation_ico = new G4RotationMatrix();

        if(directionToCenter_ico.z() > 0){
            rotation_ico->rotateX(atan2(directionToCenter_ico.y(), directionToCenter_ico.z()));
            rotation_ico->rotateY(-atan2(directionToCenter_ico.x(), directionToCenter_ico.z()));
        } else if (directionToCenter_ico.z() < 0){
            rotation_ico->rotateX(atan2(directionToCenter_ico.y(), directionToCenter_ico.z()));
            rotation_ico->rotateY(atan2(directionToCenter_ico.x(), directionToCenter_ico.z()));
            rotation_ico->rotateX(180 * deg);
        } else{
            rotation_ico->rotateX((directionToCenter_ico.y() >= 0) ? 90 * deg : -90 * deg);
            if (directionToCenter_ico.y() > 0) rotation_ico->rotateY(-atan2(directionToCenter_ico.x(), directionToCenter_ico.y()));
            else{rotation_ico->rotateY(atan2(directionToCenter_ico.x(), directionToCenter_ico.y()));
                rotation_ico->rotateY(180 * deg);
            }
        }

        new G4PVPlacement(rotation_ico, icosahedronVertices[i], logicPhMul, "physPhMul", logicInnerTank, false, i, true);
    }
    

    return physWorld;
}