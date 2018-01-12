import maya.cmds as cm

def off_translation(filename):

	print("Debut du parsing")
	f = open(filename, 'r')
	# On passe les lignes inutiles
	line = f.readline()
	line = f.readline()
	if line[0] == '#':
		line = f.readline()
		if line[0] == '#':
			line = f.readline()
	# On considere la ligne comme une liste de string separes par des espaces
	line = line.split(' ')
	nb_vert = int(line[0])
	nb_faces = int(line[1])
	list_vert = []
	list_faces = []
	# On remplit la liste des sommets
	for i in range(0,nb_vert):
		line = f.readline()
		line = line.split(' ')
		coord = [float(line[0]),float(line[1]),float(line[2])]
		list_vert.append(coord)
	line = f.readline()
	line = line.split(' ')
	# On remplit la liste des faces
	for i in range(0,nb_faces):
		index1, index2, index3 = int(line[1]), int(line[2]), int(line[3])
		points = [list_vert[index1],list_vert[index2],list_vert[index3]]
		list_faces.append(points)
		line = f.readline()
		line = line.split(' ')
	f.close()
	print("Parsing fini, debut de la creation du maillage")

	print(list_faces[0])
	# print(list_faces[1])
	obj = cm.polyCreateFacet(ch=False,p=list_faces[0])
	cm.select(obj)
	for i in range(1,nb_faces):
		cm.polyAppendVertex(a=list_faces[i])
	print("Maillage cree")
