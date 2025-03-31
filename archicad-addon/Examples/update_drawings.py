import aclib

def get_drawing_guids_from_tree(current_branch, drawing_guids):
    for navigator_item in current_branch:
        navigator_item = navigator_item['navigatorItem']
        if navigator_item['type'] == 'DrawingItem':
            drawing_guids.append(navigator_item['navigatorItemId'])
        children = navigator_item.get('children')
        if children:
            get_drawing_guids_from_tree(children, drawing_guids)

#GetElementsByType only returns drawings on the currently open layout
drawings = aclib.RunTapirCommand('GetElementsByType', {'elementType': 'Drawing'} )

print(aclib.JsonDumpDictionary(drawings))
aclib.RunTapirCommand('UpdateDrawings', drawings, debug=True)

publishing_sets = aclib.RunCommand('API.GetPublisherSetNames', {})
navigator_tree_id = {"navigatorTreeId": {"type": "LayoutBook"}}

navigator_tree = aclib.RunCommand('API.GetNavigatorItemTree', navigator_tree_id)

print(aclib.JsonDumpDictionary(navigator_tree))

drawing_gids = []

get_drawing_guids_from_tree(navigator_tree['navigatorTree']['rootItem']['children'], drawing_gids)

drawing_elements = [{"elementId": guid} for guid in drawing_gids]

#the guids of 'DrawingItem'-s are NOT valid guids for the update drawings command
aclib.RunTapirCommand('UpdateDrawings', {'elements': drawing_elements}, debug=True)









# response = aclib.RunTapirCommand('UpdateDrawings', drawings)
