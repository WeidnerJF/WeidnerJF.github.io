# Basic Dashboard application connecting to a database
# Displays data in the database in a datatable
# User is able to search for specific data in the table and then edit the data
# once the edit is completed the application will automatically update the mongodb


# Required Imports
import dash
import dash_core_components as dcc
import dash_html_components as html
import dash_table as dt
import pandas as pd
from dash.dependencies import Input, Output, State, ClientsideFunction
from bson.objectid import ObjectId
from pymongo.server_api import ServerApi
from pymongo import MongoClient

# connects to database
client = MongoClient("mongodb+srv://dbUser:Weidner2022@cluster0.pg4v2.mongodb.net/myFirstDatabase"
                     "?retryWrites=true&w=majority", server_api=ServerApi('1'))

database = client['BidSales']
# connects to Collection
collection = database['Monthly2014']

# dashboard
external_stylesheets = ['https://codepen.io/chriddyp/pen/bWLwgP.css']
app = dash.Dash(__name__, external_stylesheets=external_stylesheets,
                suppress_callback_exceptions=True)

# dashboard layout
app.layout = html.Div([
    # title for app
    html.Center(html.B(html.H1('Jacob Weidner Dashboard'))),
    # interval
    dcc.Interval(id='interval_db', interval=86400000 * 7, n_intervals=0),
    # datatable
    html.Div(id='mongo-datatable', children=([])),
    html.Div([
        html.Div(id='no-graph')
    ]),
    # Cell storage
    dcc.Store(id='changed-cell')

])


# callback for datatable population
@app.callback(Output('mongo-datatable', component_property='children'),
              [Input('interval_db', component_property='n_intervals')])
def populate_table(n_intervals):
    print(n_intervals)
    # sets database info to dataframe
    df = pd.DataFrame(list(collection.find()))

    # convert table data to string to allow editing
    df['_id'] = df['_id'].astype(str)
    print(df.head(20))
    # returns datatable for interval_db
    return [
        dt.DataTable(
            id='datatable',
            data=df.to_dict('records'),
            columns=[
                {"name": i, "id": i, "deletable": False, 'editable': False, "selectable": True} if i == '_id'
                else {"name": i, "id": i, "deletable": False, 'editable': True, "selectable": True}
                for i in df.columns

            ],

            filter_action="native",
            sort_action="native",
            sort_mode="multi",
            column_selectable=False,
            row_selectable=False,
            row_deletable=False,
            selected_columns=[],
            selected_rows=[],
            page_action="native",
            page_current=0,
            page_size=10,
        )
    ]


# clientside callback for checking which cells were modified
app.clientside_callback(
    """
    function (input,oldinput) {
        if (oldinput != null) {
            if(JSON.stringify(input) != JSON.stringify(oldinput)){
                for (i in Object.keys(input)) {
                    newArray = Object.values(input[i])
                    oldArray = Object.values(oldinput[i])
                    if (JSON.stringify(newArray)!= JSON.stringify(oldArray)){
                        entNew = Object.entries(input[i])
                        entOld = Object.entries(oldinput[i])
                        for (const j in entNew){
                            if (entNew[j][1] != entOld[j][1]) {
                                changeRef = [i, entNew[j][0]]
                                break
                            }
                        }
                    }
                }
            }
            return changeRef
        }
    }
    """,
    Output('changed-cell', 'data'),
    Input('datatable', 'data'),
    State('datatable', 'data_previous')
)


@app.callback(
    Output("no-graph", "children"),
    Input("changed-cell", "data"),
    Input("datatable", "data")

)
def update_d(cc, tabledata):
    if cc is None:
        print('Nothing changed')
        print(f'Changed-Cell: {cc}')

    # Print changed data cell
    else:
        print(f'Changed-Cell: {cc}')

        x = int(cc[0])

        # update the external database
        row_id = tabledata[x]['_id']
        col_id = cc[1]
        new_cell_data = tabledata[x][col_id]
        collection.update_one({'_id': ObjectId(row_id)},
                              {"$set": {col_id: new_cell_data}})


if __name__ == '__main__':
    app.run_server(debug=False)
